/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2009  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2010  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "eth.h"
#include "net.h"
#include "if.h"
#include "packet.h"
#include "ip.h"
#include "tcp.h"
#include "socket.h"
#include "../Synchronization.h"
#include "../errno.h"
#include "fd.h"
#include "file.h"
#include "../DynamicMemory.h"
#include "../Utility.h"
#include "../types.h"

/* Mutex for socket function */
MUTEX_CREATE (mutex_tcp_accept);
MUTEX_CREATE (mutex_tcp_read_cache);

extern netif_t netif_list;

proto_tcp_conn_t proto_tcp_conn_list;
proto_tcp_backlog_t proto_tcp_backlog_list;

static unsigned short proto_tcp_dup;
static unsigned proto_tcp_seq;
static proto_ip_t proto_ip_prealloc;
static proto_tcp_t proto_tcp_prealloc;
static packet_t packet_prealloc;
static char buf_tcp_prealloc[NET_PACKET_MTU + sizeof (proto_tcp_t) + 1];

/* prototype */
proto_tcp_conn_t *net_proto_tcp_conn_check (net_ipv4 ip_source, net_port port_source, net_ipv4 ip_dest, net_port port_dest, unsigned char *ret);
int net_proto_tcp_conn_add (fd_t *fd);
int net_proto_tcp_conn_set (proto_tcp_conn_t *conn, netif_t *eth, net_port port_source, net_ipv4 ip_dest, net_port port_dest);
int net_proto_tcp_conn_estabilish (proto_tcp_conn_t *conn, netif_t *eth, net_ipv4 ip_dest, net_port port_dest);
int net_proto_tcp_conn_estabilish_reply (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp_read_cache (proto_tcp_conn_t *conn, char *data, unsigned len);
int net_proto_tcp_read_ok (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old, unsigned len);
int net_proto_tcp_write (netif_t *eth, net_ipv4 dest, proto_tcp_t *tcp, char *data, unsigned len);
int net_proto_tcp_write_data (proto_tcp_conn_t *conn, char *data, unsigned len);
int net_proto_tcp_write_data_ok (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp_conn_disconnected (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp_conn_close (proto_tcp_conn_t *conn);
unsigned net_proto_tcp_conn_del (proto_tcp_conn_t *conn);
proto_tcp_conn_t *net_proto_tcp_conn_find (int fd);
int net_proto_tcp_backlog_add (proto_tcp_conn_t *conn, net_ipv4 ip, net_port port, unsigned seq);
int net_proto_tcp_conn_invite (proto_tcp_conn_t *conn, net_ipv4 ip, net_port port, unsigned seq);

/** TCP protocol
 *  User-friendly socket functions
 */
int net_proto_tcp_socket (fd_t *fd)
{
	return net_proto_tcp_conn_add (fd);
}

int net_proto_tcp_connect2 (int fd, sockaddr_in *addr)
{
	int ret = -1;

	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		ret = net_proto_tcp_conn_estabilish (conn, netif, addr->sin_addr, addr->sin_port);

		unsigned long stime = GetTickCount();

		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			if (!ret)
				return -1;

			for (;;) {
				Schedule ();

				/* timeout for 8s */
				if ((stime+3000) < GetTickCount())
					return -1;

				if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISHED)
					break;

				/* when connection cant be accepted succefully first time, try it again */
				if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISHERROR) {
					ret = net_proto_tcp_conn_estabilish (conn, netif, addr->sin_addr, addr->sin_port);
					conn->state = PROTO_TCP_CONN_STATE_ESTABILISH;
				}
			}
		} else
		/* non-blocking mode */
			return -1;

		ret = 0;
	}

	return ret;
}

int net_proto_tcp_connect (int fd, sockaddr_in *addr)
{
	int ret = -1;

	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);
	if (!conn)
		return -1;

	conn->port_source = (0);
	conn->seq = (0);
	conn->fFirstPacket = (NULL);
	conn->fLastPacket = (NULL);
	conn->fFirstSentPacket = (NULL);
	conn->fLastSentPacket = (NULL);
	conn->fState = (TCP_SOCKET_STATE_INITIAL);
	conn->fRemoteState = (TCP_SOCKET_STATE_INITIAL);

	netif_t *netif = netif_findbyname ("eth0");
	{
		conn->ip_source = netif->ip;
		conn->ip_dest = addr->sin_addr;
		conn->port_dest = addr->sin_port;

	
		conn->seq = _rand32();
		conn->port_source = 0xC000 + (_rand14() & ~0xc000);
		conn->ack = 0;
		conn->fNextSequence = 0;
		conn->checkedask = 1;
		// send SYN
		struct TCPPacket* packet = NEW(sizeof(struct TCPPacket));
		if (packet == NULL)
			return -1;

		packet->fNext = NULL;

		int	error = SetTo(packet,NULL, 0, conn->ip_source, conn->port_source, conn->ip_dest, conn->port_dest,
				conn->seq, conn->ack, TCP_SYN);
		if (error != 0)
		{
			TRACE("Call SetTo error %d\n",error);

			DEL( packet );
			return error;
		}
		error = SSend(conn,packet,true);
		if (error != 0)
		{
			TRACE("_Send error %d\n",error);
			return error;
		}
		conn->fState = TCP_SOCKET_STATE_SYN_SENT;
		conn->seq++;

		// receive SYN-ACK
		error = _WaitForState(conn,TCP_SOCKET_STATE_OPEN, 1000000LL);
		if (error != 0) {
			TRACE("no SYN-ACK received\n");
			return error;
		}
		TRACE("SYN-ACK received\n");

		return 1;
	}

	return 0;
}

int net_proto_tcp_send (int fd, char *msg, unsigned size)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	int len = Write(conn,msg,size);

	return len;
}

int net_proto_tcp_recv (int fd, char *msg, unsigned size)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -3;

	int rsize;
	Read(conn,msg,size,&rsize,3000);

	return rsize;
}

int net_proto_tcp_close (int fd)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	if (conn->state == PROTO_TCP_CONN_STATE_DISCONNECTED)
		return 0;

	return net_proto_tcp_conn_close (conn);
}

int net_proto_tcp_fcntl (int fd, int cmd, long arg)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	switch (cmd) {
		case F_GETFL:
			return conn->flags;
		case F_SETFL:
			conn->flags = arg;
			return 0;
	}

	return -1;
}

int net_proto_tcp_bind (int fd, sockaddr_in *addr, socklen_t len)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		net_proto_tcp_conn_set (conn, netif, addr->sin_port, 0, 0);
		conn->bind = 1;
		return 0;
	}

	return -1;
}

int net_proto_tcp_listen (int fd, int backlog)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	return 0;
}

int net_proto_tcp_accept (int fd, sockaddr_in *addr, socklen_t *addrlen)
{
	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	unsigned i = 0;

	proto_tcp_backlog_t *backlog = 0;

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		while (!i) {
			for (backlog = proto_tcp_backlog_list.next; backlog != &proto_tcp_backlog_list; backlog = backlog->next) {
				if (backlog->conn == conn) {
					i = 1;
					break;
				}
			}
	
			Schedule ();
		}
	} else {
	/* non-blocking mode */
		for (backlog = proto_tcp_backlog_list.next; backlog != &proto_tcp_backlog_list; backlog = backlog->next) {
			if (backlog->conn == conn) {
				i = 1;
				break;
			}
		}
	}

	//Printf ("accept ()\n");

	if (!i)
		return -1;

	net_ipv4 ip = backlog->ip;
	net_port port = backlog->port;
	unsigned seq = backlog->seq;

	int ret = net_proto_tcp_conn_invite (conn, ip, port, seq);

	/* remove from queue accepted connection */
	backlog->next->prev = backlog->prev;
	backlog->prev->next = backlog->next;

	DEL (backlog);

	if (ret < 1)
		return ret;
	
	fd_t *fd_new = fd_create (FD_SOCK);
	
	if (!fd_new)
		return 0;
	
	net_proto_tcp_conn_add (fd_new);

	proto_tcp_conn_t *conn_new = net_proto_tcp_conn_find (fd_new->id);

	if (!conn_new)
		return -1;

	net_proto_tcp_conn_set (conn_new, conn->netif, conn->port_source, conn->netif->ip, port);

	conn_new->ip_dest = ip;
	
	addr->sin_addr = ip;
	addr->sin_port = port;
	addr->sin_family = AF_INET;
	
	/* wait for ACK */
	unsigned long stime = GetTickCount();
	while (conn_new->state != PROTO_TCP_CONN_STATE_READY) {
		if ((stime+10) < GetTickCount())
			return -1;
	  
		Schedule ();
	}

	return fd_new->id;
}

int net_proto_tcp_select (int readfd, int writefd, int exceptfd)
{
	int fd = -1;
  
	if (readfd)
		fd = readfd;
	else if (writefd)
		fd = writefd;
	else if (exceptfd)
		fd = exceptfd;

	proto_tcp_conn_t *conn = net_proto_tcp_conn_find (fd);

	if (!conn)
		return -1;

	/* check for incoming connections */
	if (conn->bind) {
		proto_tcp_backlog_t *backlog = 0;

		for (backlog = proto_tcp_backlog_list.next; backlog != &proto_tcp_backlog_list; backlog = backlog->next) {
			if (backlog->conn == conn)
				return 1;
		}

		return 0;
	}
	
	/* Is socket closed ? */
	if (conn->state == PROTO_TCP_CONN_STATE_CLOSE)
		return 1;
	
	/* Are some data available ? */
	if (conn->len)
		return 1;
	
	return 0;
}

/** TCP protocol
 *  Hardcore code - syn, ack, psh, fin, etc :P
 */
unsigned net_proto_tcp_handler (packet_t *packet, proto_ip_t *ip, char *buf, unsigned len)
{
	proto_tcp_t *tcp = (proto_tcp_t *) buf;

	unsigned char ret = 0;

	/* First check ip address and ports, that we want this data */
	proto_tcp_conn_t *conn = net_proto_tcp_conn_check (ip->ip_dest, tcp->port_dest, ip->ip_source, tcp->port_source, &ret);

	if (!conn && ret == 0)
	{
		Printf ("net_proto_tcp_conn_check fail %d\n",ret);
		return 1;
	}

	//Printf ("tcp->flags: 0x%x, ret: %d, conn: 0x%x, fd: %d\n", tcp->flags, ret, conn, conn->fd);

	/* connection from client before accept () */
	if (ret == 1) {
		/* client want connect to our server */
		if (tcp->flags == 0x02) {
			Lock (&mutex_tcp_accept);

			unsigned ret = net_proto_tcp_backlog_add (conn, ip->ip_source, tcp->port_source, tcp->seq);

			Unlock (&mutex_tcp_accept);

			return ret; 
		}

		return 1;
	}

	unsigned data_cache = 0;

	/* data received */
	if (tcp->flags & 0x08) {
		Lock (&mutex_tcp_read_cache);

		/* needed for calculate real offset from 4bit number */
		unsigned offset = tcp->data_offset * 4;

		/* now calculate accurate length of tcp _data_ */
		unsigned size = swap16 (ip->total_len) - (offset + (ip->head_len*4));

		//Printf ("2.1>>>>total_len  %d : offset %d ip->head_len  %d\n", swap16 (ip->total_len), offset, (offset + (ip->head_len*4)));
		//Printf ("2.>>>>offset  %d : size %d len  %d\n", offset, size, len);

		net_proto_tcp_read_cache (conn, buf+offset, size);
		/* send ack */
		net_proto_tcp_read_ok (conn, ip, tcp, size);

		data_cache = 1;
		Unlock (&mutex_tcp_read_cache);
	}

	/* sended data was delivered succefully / ACK */
	if (tcp->flags & 0x10) {
		/* HACK: It is interesting, that no push flag, and there could be some data to read */
		Lock (&mutex_tcp_read_cache);

		/* needed for calculate real offset from 4bit number */
		unsigned offset = tcp->data_offset * 4;

		/* now calculate accurate length of tcp _data_ */
		unsigned size = swap16 (ip->total_len) - (offset + (ip->head_len*4));

		//Printf ("2.>>>>offset  %d : size %d len  %d\n", offset, size, len);


		/* there are data for read */
		if (size) {
			/* data was cached, so no need cache it again */
			if (!data_cache) {
				//kPrintf (">>>>2 %d : %d : %d\n", offset, size, len);
				net_proto_tcp_read_cache (conn, buf+offset, size);
				/* send ack */
				net_proto_tcp_read_ok (conn, ip, tcp, size);
			}
		} else {
			/* when data are'nt available, lets normal work - acknowledgement */
			if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISH) {
				conn->state = PROTO_TCP_CONN_STATE_ESTABILISHERROR;
				net_proto_tcp_write_data_ok (conn, ip, tcp);
			} else
				net_proto_tcp_write_data_ok (conn, ip, tcp);
		}

		Unlock (&mutex_tcp_read_cache);

	}

	/* connection estabilish respond */
	if (tcp->flags == 0x12) {
		return net_proto_tcp_conn_estabilish_reply (conn, ip, tcp);
	}

	/* another side close connection */
	if (tcp->flags & 0x01) {
		Printf("TCP -> fd %d connection closed by remote host\n", conn->fd);
		net_proto_tcp_conn_disconnected (conn, ip, tcp);			// FIXME: problem in server with hangind the whole program
	}

	/* another side close connection */
	if (tcp->flags & 0x04) {
		Printf("TCP -> fd %d connection reseted by peer\n", conn->fd);
		net_proto_tcp_conn_disconnected (conn, ip, tcp);
	}

	return 1;
}

int net_proto_tcp_read_cache (proto_tcp_conn_t *conn, char *data, unsigned len)
{
	if (!data || !conn || !len)
		return 0;

	if (!conn->len) {
		conn->data = (char *) NEW (sizeof (char) * (len + 1));

		memcpy (conn->data, data, len);
	} else {
		char *newdata = (char *) NEW (sizeof (char) * (conn->len + len + 1));

		if (!newdata)
			return 0;

		memcpy (newdata, conn->data, conn->len);
		memcpy (newdata+conn->len, data, len);

		DEL (conn->data);

		conn->data = newdata;
	}

	if (!conn->data)
		return -1;

	//Printf ("\n3.DATA: %d - fd %d\n", len, conn->fd);
	
	conn->len += len;

	conn->data[conn->len] = '\0';

	return conn->len;
}

int net_proto_tcp_read_ok (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old, unsigned len)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source; // increase client port too
	tcp->port_dest = conn->port_dest;

	tcp->seq = tcp_old->ack;

	/* this small thing did bad work when e.g. 10 10 10 fe get to calculation,
		it returns 10 10 10 08, no 10 10 11 08 */
	unsigned seq = swap32 (tcp_old->seq);
	seq += len;

	tcp->ack = swap32 (seq);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x10;
	tcp->window = tcp_old->window + swap16 (64);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	/*if (proto_tcp_dup == tcp_old->checksum) {
		Printf ("Duplicated packet :)");
		Sleep (150);
	}*/

	if (ret) {
		//Printf ("read_ok: seq: 0x%x | ack: 0x%x\n", conn->seq, conn->ack);

		conn->seq = tcp->ack;
		conn->ack = tcp->seq;

		//Printf ("read_ok2: seq: 0x%x | ack: 0x%x\n", conn->seq, conn->ack);

		proto_tcp_dup = tcp_old->checksum;

		conn->cross = 1;
	}

	DEL (tcp);

	return ret;
}

int net_proto_tcp_write (netif_t *eth, net_ipv4 dest, proto_tcp_t *tcp, char *data, unsigned len)
{
	if (!eth || !tcp)
		return 0;

	if (len)
		if (!data)
			return 0;

	mac_addr_t mac_dest;
	unsigned get = arp_cache_get (dest, &mac_dest);

	if (!get) {
		arp_send_request (eth, dest);

		unsigned i = 0;
		/* 100ms for waiting on ARP reply */
		while (i < 100) {
			get = arp_cache_get (dest, &mac_dest);

			if (get)
				break;

			/* TODO: make better waiting for ARP reply */
			Sleep (10);

			Schedule ();
	
			i ++;
		}

		if (!get)
			return 0;
	}

	/* packet header */
	packet_t *packet = &packet_prealloc;

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, eth->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV4;

	/* ip layer */
	proto_ip_t *ip = &proto_ip_prealloc;

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 4;
	ip->head_len = 5;

	ip->flags = 0;
	ip->frag = 0;
	ip->ttl = 64;
	ip->checksum = 0;
	ip->proto = NET_PROTO_IP_TYPE_TCP;
	ip->ip_source = eth->ip;
	ip->ip_dest = dest;

	unsigned l = (tcp->data_offset*4);

	/* calculate total length of packet (tcp/ip) */
	ip->total_len = swap16 (l+sizeof (proto_tcp_t)+len);

	
	if (len+l > NET_PACKET_MTU + sizeof (proto_tcp_t)) {
		Printf("TCP -> data lenght is exceed: %d/%d bytes", len+l, NET_PACKET_MTU + sizeof (proto_tcp_t) + 1);
		return 0;
	}
	
	char *buf_tcp = (char *) &buf_tcp_prealloc; //(char *) NEW ((len+l+1) * sizeof (char));

	if (!buf_tcp)
		return 0;

	memcpy (buf_tcp, (char *) tcp, l);

	if (len)
		memcpy (buf_tcp+l, data, len);

//	buf_tcp[l+len] = '\0';

	/* calculate checksum and put it to tcp header */
	proto_tcp_t *tcp_ = (proto_tcp_t *) buf_tcp;

	tcp_->checksum = checksum16_tcp (eth->ip, dest, buf_tcp, l+len);

	
	/* send tcp header+data to ip layer */
	unsigned ret = net_proto_ip_send (eth, packet, ip, (char *) buf_tcp, l+len);

	//DEL (buf_tcp);

	return ret;
}

int net_proto_tcp_write_data (proto_tcp_conn_t *conn, char *data, unsigned len)
{
	if (!conn || !len || !data)
		return 0;

	proto_tcp_t *tcp = &proto_tcp_prealloc;

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source;
	tcp->port_dest = conn->port_dest;

	unsigned seq = swap32 (proto_tcp_seq);
	seq ++;

	//tcp->ack = swap32 (seq);

	proto_tcp_seq = swap32 (seq);

	if (!conn->cross) {
		tcp->seq = conn->seq;
		tcp->ack = conn->ack;
	} else {
		tcp->seq = conn->ack;
		tcp->ack = conn->seq;

		conn->cross = 0;
	}

//	Printf ("TCP -> seq: 0x%x | ack: 0x%x | proto_seq: 0x%x\n", tcp->seq, tcp->ack, proto_tcp_seq);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x18;
	tcp->window = swap16 (32768);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, conn->ip_dest, tcp, data, len);

	if (ret)
		conn->state = PROTO_TCP_CONN_STATE_WAIT;

	return ret;
}

int net_proto_tcp_write_data_ok (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	/* cross - because we change now sides :) */
	conn->seq = tcp_old->ack;
	conn->ack = tcp_old->seq;

//	Printf ("data_ok: seq: 0x%x | ack: 0x%x\n", tcp_old->ack, tcp_old->seq);

	conn->state = PROTO_TCP_CONN_STATE_READY;

	return 1;
}

/* try connect to server */
int net_proto_tcp_conn_estabilish (proto_tcp_conn_t *conn, netif_t *eth, net_ipv4 ip_dest, net_port port_dest)
{
	if (!conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = swap16 (netif_port_get ()); // increase client port too
	tcp->port_dest = port_dest;

	/* setup new connection */
	net_proto_tcp_conn_set (conn, eth, tcp->port_source, ip_dest, tcp->port_dest);

	tcp->seq = conn->seq;
	tcp->ack = conn->ack;

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x02;
	tcp->window = swap16 (32792);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (eth, ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (!ret) {
		net_proto_tcp_conn_del (conn);
		return 0;
	}

	return 1;
}

int net_proto_tcp_conn_estabilish_reply (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source; // increase client port too
	tcp->port_dest = conn->port_dest;

	tcp->seq = tcp_old->ack;
	tcp->ack = tcp_old->seq + swap32 (1);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x10;
	tcp->window = tcp_old->window + swap16 (64);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	if (ret) {
		conn->seq = tcp->seq;
		conn->ack = tcp->ack;
		conn->state = PROTO_TCP_CONN_STATE_ESTABILISHED;
		
		//Printf("TCP -> fd %d connected to server succefully\n", conn->fd);
	}

	DEL (tcp);

	return ret;
}

int net_proto_tcp_conn_disconnected (proto_tcp_conn_t *conn, proto_ip_t *ip_old, proto_tcp_t *tcp_old)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	if (conn->state == PROTO_TCP_CONN_STATE_CLOSE)
		return net_proto_tcp_conn_del (conn);

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source; // increase client port too
	tcp->port_dest = conn->port_dest;

	tcp->seq = tcp_old->ack;
	tcp->ack = tcp_old->seq + swap32 (1);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x10;
	tcp->window = tcp_old->window + swap16 (64);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (ret)
		conn->state = PROTO_TCP_CONN_STATE_CLOSE; //ret = net_proto_tcp_conn_del (conn);
	else
		return -1;

	return ret;
}

int net_proto_tcp_conn_close (proto_tcp_conn_t *conn)
{
	return 0;	/* FIXME: close () is faster then send () sometime :-B */
  
	if (!conn)
		return 0;

	if (conn->state == PROTO_TCP_CONN_STATE_CLOSE)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source; // increase client port too
	tcp->port_dest = conn->port_dest;

	tcp->seq = conn->seq;
	tcp->ack = conn->ack;

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x11;
	tcp->window = swap16 (32832);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (ret)
		conn->state = PROTO_TCP_CONN_STATE_CLOSE;

	return ret;
}

/* client wants to connect */
net_proto_tcp_conn_invite (proto_tcp_conn_t *conn, net_ipv4 ip, net_port port, unsigned seq)
{	
	if (!conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source;
	tcp->port_dest = port;

	tcp->seq = proto_tcp_seq ++;
	tcp->ack = seq + swap32 (1);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x12;
	tcp->window = swap16 (5816);
	tcp->checksum = 0;

	int ret = net_proto_tcp_write (conn->netif, ip, tcp, 0, 0);
	
	DEL (tcp);

 	if (ret > 0)
		conn->state = PROTO_TCP_CONN_STATE_READY;
	else {
		Printf ("wtf ?\n");
		return -1;
	}
	return 1;
}

/* Create new TCP connection */
int net_proto_tcp_conn_add (fd_t *fd)
{
	proto_tcp_conn_t *conn;

	/* alloc and init context */
	conn = (proto_tcp_conn_t *) NEW (sizeof (proto_tcp_conn_t));

	if (!conn) {
		errno_set (ENOMEM);
		return -1;
	}

	memset (conn, 0, sizeof (proto_tcp_conn_t));

	conn->flags = 0;
	conn->bind = 0;

	conn->fd = fd->id;

	conn->cross = 0;

	/* add into list */
	conn->next = &proto_tcp_conn_list;
	conn->prev = proto_tcp_conn_list.prev;
	conn->prev->next = conn;
	conn->next->prev = conn;

	return 0;
}

/* Setup new connection */
int net_proto_tcp_conn_set (proto_tcp_conn_t *conn, netif_t *eth, net_port port_source, net_ipv4 ip_dest, net_port port_dest)
{
	if (!conn || !eth)
		return 0;

	conn->ip_source = eth->ip;
	conn->ip_dest = ip_dest;

	conn->port_source = port_source;
	conn->port_dest = port_dest;

	proto_tcp_seq += swap32 (1);

	conn->seq = proto_tcp_seq ++;
	conn->ack = 0;

	conn->state = PROTO_TCP_CONN_STATE_ESTABILISH;

	conn->netif = eth;

	conn->offset = 0;
	conn->len = 0;
	conn->data = 0;

	return 1;
}

/* Delete existing connection from list */
unsigned net_proto_tcp_conn_del (proto_tcp_conn_t *conn)
{
	if (!conn)
		return 0;

	conn->state = PROTO_TCP_CONN_STATE_DISCONNECTED;

	if (conn->len)
		DEL (conn->data);

	conn->len = 0;

	conn->next->prev = conn->prev;
	conn->prev->next = conn->next;

	DEL (conn);

	return 1;
}

proto_tcp_conn_t *net_proto_tcp_conn_check (net_ipv4 ip_source, net_port port_source, net_ipv4 ip_dest, net_port port_dest, unsigned char *ret)
{
	*ret = 0;
	proto_tcp_conn_t *conn = NULL;
	proto_tcp_conn_t *conn_ret = NULL;
	//Printf ("-------------------------\n");
	for (conn = proto_tcp_conn_list.next; conn != &proto_tcp_conn_list; conn = conn->next) {
		/*Printf ("-> ");
		net_proto_ip_print (conn->ip_source);
		Printf (" / ");
		net_proto_ip_print (ip_source);
		Printf ("\n2> ");
		net_proto_ip_print (conn->ip_dest);
		Printf (" / ");
		net_proto_ip_print (ip_dest);
		Printf ("\nporty: %d / %d\n", swap16 (conn->port_source), swap16 (port_source));
		Printf ("porty2: %d / %d\n", swap16 (conn->port_dest), swap16 (port_dest));*/

		if (conn->ip_source == ip_source && conn->port_source == port_source) {
			if (conn->ip_dest == ip_dest && conn->port_dest == port_dest) {
				*ret = 2;
				return conn;
			}

			*ret = 1;

			conn_ret = conn;
		}
	}

	if (*ret == 1)
		if (!conn_ret->bind) {
			conn_ret = 0;

			for (conn = proto_tcp_conn_list.next; conn != &proto_tcp_conn_list; conn = conn->next) {
				if (conn->bind) {
					if (conn->ip_source == ip_source && conn->port_source == port_source)
						conn_ret = conn;
				}
			}
		}
	
	return conn_ret;
}

proto_tcp_conn_t *net_proto_tcp_conn_find (int fd)
{
	proto_tcp_conn_t *conn = NULL;
	for (conn = proto_tcp_conn_list.next; conn != &proto_tcp_conn_list; conn = conn->next) {
		if (conn->fd == fd)
			return conn;
	}
	
	return 0;
}

/* Create new TCP backlog stamp */
int net_proto_tcp_backlog_add (proto_tcp_conn_t *conn, net_ipv4 ip, net_port port, unsigned seq)
{
	if (!conn)
		return 0;

	proto_tcp_backlog_t *backlog;

	/* alloc and init context */
	backlog = (proto_tcp_backlog_t *) NEW (sizeof (proto_tcp_backlog_t));

	if (!backlog)
		return 0;

	backlog->conn = conn;
	backlog->ip = ip;
	backlog->port = port;
	backlog->seq = seq;

	/* add into list */
	backlog->next = &proto_tcp_backlog_list;
	backlog->prev = proto_tcp_backlog_list.prev;
	backlog->prev->next = backlog;
	backlog->next->prev = backlog;
	
	return conn->fd;
}

/* init of tcp protocol */
unsigned init_net_proto_tcp ()
{
	proto_tcp_conn_list.next = &proto_tcp_conn_list;
	proto_tcp_conn_list.prev = &proto_tcp_conn_list;

	proto_tcp_backlog_list.next = &proto_tcp_backlog_list;
	proto_tcp_backlog_list.prev = &proto_tcp_backlog_list;

	proto_tcp_dup = 0x0;
	proto_tcp_seq = 0x1;

	return 1;
}


struct ChainBuffer *DetachNext(struct ChainBuffer *buffer)
{
	if (!buffer->fNext)
		return NULL;

	struct ChainBuffer *next = buffer->fNext;

	buffer->fNext = NULL;
	next->fFlags |= CHAIN_BUFFER_HEAD;
	buffer->fTotalSize = buffer->fSize;

	return next;
}

// Append
void Append(struct ChainBuffer *buffer,struct ChainBuffer *next)
{
	if (!next)
		return;

	if (buffer->fNext)
		Append(buffer->fNext,next);
	else
		buffer->fNext = next;

	buffer->fTotalSize = buffer->fSize + buffer->fNext->fTotalSize;
}

// Flatten
void Flatten(struct ChainBuffer *this,void *_buffer)
{
	unsigned char *buffer = (unsigned char*)_buffer;
	{
		if (this->fData && this->fSize > 0)
		{
			memcpy(buffer, this->fData, this->fSize);
			buffer += this->fSize;
		}

		if (this->fNext)
			Flatten(this->fNext,buffer);
	}
}

// _Init
void Init(struct ChainBuffer *this,void *data, uint32_t size, struct ChainBuffer *next, uint32_t flags)
{
	this->fFlags = flags | CHAIN_BUFFER_HEAD;
	this->fSize = size;
	this->fTotalSize = this->fSize;
	this->fData = data;
	this->fNext = NULL;
	Append(this,next);
}

// _Destroy
void Destroy(struct ChainBuffer *this)
{
	struct ChainBuffer *next = this->fNext;
	this->fNext = NULL;
	if ((this->fFlags & CHAIN_BUFFER_FREE_DATA) && this->fData) {
		DEL(this->fData);
		this->fData = NULL;
	}

	if (!(this->fFlags & CHAIN_BUFFER_EMBEDDED_DATA))
		this->fSize = 0;
	this->fTotalSize = this->fSize;

	if (next) {
		if (next->fFlags & CHAIN_BUFFER_ON_STACK)
			Destroy(next);
		else
			DEL (next);
	}
}

void ChainBuffer(struct ChainBuffer *this,void *data, uint32_t size, struct ChainBuffer *next,bool freeData)
{
	Init(this,data, size, next,
		CHAIN_BUFFER_ON_STACK | (freeData ? CHAIN_BUFFER_FREE_DATA : 0));
}

//-------------------------------------------------------------------------------------------------
unsigned int _rand32(void)
{
	static unsigned int next = 0;
	if (next == 0)
		next = GetTickCount() / 1000000;

	next = (next >> 1) ^ (unsigned int)((0 - (next & 1U)) & 0xd0000001U);
		// characteristic polynomial: x^32 + x^31 + x^29 + x + 1
	return next;
}

unsigned short _rand14(void)
{
	// TODO: Find suitable generator polynomial.
	return _rand32() & 0x3fff;
}

int SetTo(struct TCPPacket *packet,const void* data, int size, net_ipv4 sourceAddress,
	uint16_t sourcePort, net_ipv4 destinationAddress, uint16_t destinationPort,
	uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint8_t flags)
{
	if (data == NULL && size > 0)
		return -1;

	if (size > 0) {
		packet->fData = NEW(size);
		if (packet->fData == NULL)
			return -1;
		memcpy(packet->fData, data, size);
	} else
		packet->fData = NULL;

	packet->fSize = size;
	packet->fSourceAddress = sourceAddress;
	packet->fSourcePort = sourcePort;
	packet->fDestinationAddress = destinationAddress;
	packet->fDestinationPort = destinationPort;
	packet->fSequenceNumber = sequenceNumber;
	packet->fAcknowledgmentNumber = acknowledgmentNumber;
	packet->fFlags = flags;
	packet->fNext = NULL;

	return 0;
}


bool ProvidesSequenceNumber(struct TCPPacket *packet,uint32_t sequenceNumber)
{
	// TODO PAWS
	return packet->fSequenceNumber <= sequenceNumber
		&& packet->fSequenceNumber + packet->fSize > sequenceNumber;
}

int WindowSize()
{
	return 4096;
}


int Send(netif_t *eth,uint16_t sourcePort, net_ipv4 destinationAddress,
	uint16_t destinationPort, uint32_t sequenceNumber,
	uint32_t acknowledgmentNumber, uint8_t flags, uint16_t windowSize,
	struct ChainBuffer* buffer)
{
	if (buffer == NULL)
	{
		TRACE("Send buffer nul\n");
		return -1;
	}

	struct proto_tcp_t header;
	struct ChainBuffer headerBuffer;

	ChainBuffer(&headerBuffer,&header, sizeof(header), buffer,false);

	memset(&header, 0, sizeof(header));
	header.port_source = htons(sourcePort);
	header.port_dest = (destinationPort);
	header.seq = htonl(sequenceNumber);
	header.ack = htonl(acknowledgmentNumber);
	header.data_offset = 5;
	header.flags = flags;
	header.window = htons(windowSize);
	header.res = 0;
	header.urgentPointer = 0;
	header.checksum = 0;

	header.checksum = htons(_ChecksumBuffer(&headerBuffer,
			eth->ip, destinationAddress,
		headerBuffer.fTotalSize));

//	TRACE("data + tcp size %d\n", headerBuffer.fTotalSize);

	return ___Send(destinationAddress, IPPROTO_TCP, &headerBuffer);
}

void
_EnqueueOutgoingPacket(struct TCPPacket* packet,struct proto_tcp_conn_context *this)
{
	if (this->fLastSentPacket != NULL) {
		this->fLastSentPacket->fNext = (packet);
		this->fLastSentPacket = packet;
	} else {
		this->fFirstSentPacket = this->fLastSentPacket = packet;
	}
}

int
Close(struct proto_tcp_conn_context *context)
{
	// send FIN
	struct TCPPacket* packet = NEW( sizeof(struct TCPPacket));
	if (packet == NULL)
		return -1;
	packet->fNext = NULL;

	int error = SetTo(packet,NULL, 0, context->ip_source, context->port_source, context->ip_dest,
			context->port_dest, context->seq, context->ack, TCP_FIN | TCP_ACK);
	if (error != 0) {
		DEL (packet);
		return error;
	}
	error = SSend(context,packet,true);
	if (error != 0)
		return error;

	context->fState = TCP_SOCKET_STATE_FIN_SENT;
	TRACE("FIN sent\n");

	error = _WaitForState(context,TCP_SOCKET_STATE_CLOSED, 1000000LL);
	if (error != 0)
		return error;

	return 0;
}

int
Write(struct proto_tcp_conn_context *context,const void* buffer, int bufferSize)
{
	if (buffer == NULL || bufferSize == 0)
		return -1;

	// TODO: Check for MTU and create multiple packets if necessary.

	struct TCPPacket* packet = NEW(sizeof(struct TCPPacket));
	if (packet == NULL)
		return -1;

	while(context->checkedask != context->seq);

	int error = SetTo(packet,buffer, bufferSize, context->ip_source, context->port_source,
			context->ip_dest, context->port_dest, context->seq, context->ack,TCP_ACK);
	if (error != 0) {
		DEL (packet);
		return error;
	}
	return SSend(context,packet,true);
}


void
Acknowledge(struct proto_tcp_conn_context *context,uint32_t number)
{
		// dequeue packets
	struct TCPPacket* packet;
	for (packet = context->fFirstSentPacket; packet != NULL;
			packet = context->fFirstSentPacket) {
		if (packet->fSequenceNumber >= number)
			return;

		context->fFirstSentPacket = packet->fNext;
		DEL (packet);
	}
	context->fLastSentPacket = NULL;
}

void
ProcessPacket(struct proto_tcp_conn_context *context,struct TCPPacket* packet)
{
	if ((packet->fFlags & TCP_FIN) != 0)
	{
		context->fRemoteState = TCP_SOCKET_STATE_FIN_SENT;
		TRACE("FIN received\n");
		_Ack(context);
	}

	if (context->fState == TCP_SOCKET_STATE_SYN_SENT)
	{
		if ((packet->fFlags & TCP_SYN) != 0
				&& (packet->fFlags & TCP_ACK) != 0)
		{
			context->fNextSequence = context->ack = packet->fSequenceNumber + 1;
			context->fRemoteState = TCP_SOCKET_STATE_SYN_SENT;
			DEL (packet);
			_Ack(context);
			context->fState = context->fRemoteState = TCP_SOCKET_STATE_OPEN;
			TRACE("TCP_SOCKET_STATE_OPEN state\n");
			return;
		}
	}
	else if (context->fState == TCP_SOCKET_STATE_OPEN)
	{
	}
	else if (context->fState == TCP_SOCKET_STATE_FIN_SENT)
	{
		if ((packet->fFlags & TCP_ACK) != 0)
		{
			TRACE("FIN-ACK received\n");
			if (context->fRemoteState == TCP_SOCKET_STATE_FIN_SENT)
				context->fState = TCP_SOCKET_STATE_CLOSED;
		}
	}

	if (packet->fSize == 0) {
		//TRACE("TCPSocket::ProcessPacket(): not queuing due to lack of data\n");
		DEL (packet);
		return;
	}

//	Printf("\n%s\n",packet->fData);

	// For now rather protect us against being flooded with packets already
	// acknowledged. "If it's important, they'll send it again."
	// TODO PAWS
	if (packet->fSequenceNumber < context->ack) {
		DEL (packet);
		return;
	}

	if (context->fLastPacket == NULL) {
		// no packets enqueued
		packet->fNext = (NULL);
		context->fFirstPacket = context->fLastPacket = packet;
	} else if (context->fLastPacket->fSequenceNumber < packet->fSequenceNumber) {
		// enqueue in back
		packet->fNext = (NULL);
		context->fLastPacket->fNext = (packet);
		context->fLastPacket = packet;
	} else if (context->fFirstPacket->fSequenceNumber > packet->fSequenceNumber) {
		// enqueue in front
		packet->fNext = (context->fFirstPacket);
		context->fFirstPacket = packet;
	} else if (context->fFirstPacket->fSequenceNumber == packet->fSequenceNumber) {
		DEL (packet);
		return;
	} else {
		// enqueue in middle
		struct TCPPacket* queuedPacket ;
		for (queuedPacket = context->fFirstPacket; queuedPacket != NULL;
				queuedPacket = queuedPacket->fNext) {
			if (queuedPacket->fSequenceNumber == packet->fSequenceNumber) {
				// we may be waiting for a previous packet
				DEL (packet);
				return;
			}
			if (queuedPacket->fNext->fSequenceNumber
					> packet->fSequenceNumber) {
				packet->fNext = (queuedPacket->fNext);
				queuedPacket->fNext = (packet);
				break;
			}
		}
	}

	while (packet != NULL && packet->fSequenceNumber == context->ack) {
		context->ack = packet->fSequenceNumber + packet->fSize;
		packet = packet->fNext;
	}
}


struct TCPPacket*
_PeekPacket(struct proto_tcp_conn_context *context)
{
	struct TCPPacket* packet;
	for (packet = context->fFirstPacket; packet != NULL;packet = packet->fNext)
	{
		if (ProvidesSequenceNumber(packet,context->fNextSequence))
			return packet;
	}
	return NULL;
}


struct TCPPacket*
_DequeuePacket(struct proto_tcp_conn_context *context)
{
	//TRACE("TCPSocket::DequeuePacket()\n");
	if (context->fFirstPacket == NULL)
		return NULL;

	if (ProvidesSequenceNumber(context->fFirstPacket,context->fNextSequence)) {
		struct TCPPacket* packet = context->fFirstPacket;
		context->fFirstPacket = packet->fNext;
		if (context->fFirstPacket == NULL)
			context->fLastPacket = NULL;
		packet->fNext = (NULL);
		return packet;
	}

	struct TCPPacket* packet;
	for (packet = context->fFirstPacket;
			packet != NULL && packet->fNext != NULL;
			packet = packet->fNext) {
		if (ProvidesSequenceNumber(packet->fNext,context->fNextSequence)) {
			struct TCPPacket* nextPacket = packet->fNext;
			packet->fNext = (nextPacket->fNext);
			if (context->fLastPacket == nextPacket)
				context->fLastPacket = packet;
			return nextPacket;
		}
	}
	Printf("dequeue failed!\n");
	return NULL;
}



int __Send(uint16_t sourcePort, net_ipv4 destinationAddress,uint16_t destinationPort, uint32_t sequenceNumber,
	uint32_t acknowledgmentNumber, uint8_t flags, uint16_t windowSize,struct ChainBuffer* buffer)
{
	if (buffer == NULL)
		return -1;

	netif_t *netif = netif_findbyname ("eth0");

	struct proto_tcp_t header;
	struct ChainBuffer headerBuffer;
	ChainBuffer(&headerBuffer,&header, sizeof(header), buffer,false);
	memset(&header, 0, sizeof(header));
	header.port_source = htons(sourcePort);
	header.port_dest = htons(destinationPort);
	header.seq = htonl(sequenceNumber);
	header.ack = htonl(acknowledgmentNumber);
	header.data_offset = 5;
	header.flags = flags;
	header.window = htons(windowSize);

	header.checksum = 0;
	header.checksum = htons(_ChecksumBuffer(&headerBuffer,
			netif->ip, destinationAddress,
		headerBuffer.fTotalSize));

	return ___Send(destinationAddress, IPPROTO_TCP, &headerBuffer);
}

int ___Send(net_ipv4 destination, uint8_t protocol, struct ChainBuffer *buffer)
{
	if (!buffer)
	{
		return -1;
	}

	netif_t *netif = netif_findbyname ("eth0");

	// prepare header
	struct proto_ip_t header;
	struct ChainBuffer headerBuffer;
	ChainBuffer(&headerBuffer,&header, sizeof(header), buffer,false);

	header.head_len = 5;	// 5 32 bit words, no options
	header.ver = IP_PROTOCOL_VERSION_4;
	header.res0 = 0;
	header.total_len =htons(headerBuffer.fTotalSize);
	header.ident = 0;
	header.flags = htons(IP_DONT_FRAGMENT);
	header.ttl = IP_DEFAULT_TIME_TO_LIVE;
	header.proto = protocol;
	header.checksum = 0;
	header.ip_source = (netif->ip);
	header.ip_dest = (destination);

	// compute check sum
	header.checksum = htons(_Checksum(&header));

	// get target MAC address
	mac_addr_t mac_dest;
	
	unsigned get = arp_cache_get (destination, &mac_dest);

	if (!get)
	{
		arp_send_request (netif, destination);

		unsigned i = 0;
		/* 100ms for waiting on ARP reply */
		while (i < 100)
		{
			get = arp_cache_get (destination, &mac_dest);

			if (get)
				break;

				/* TODO: make better waiting for ARP reply */
			Sleep (10);

			Schedule ();

			i ++;
		}

		if (!get)
		{
			Printf("fail arp_cache_get %d\n",destination);
			return 0;
		}
	}

//	Printf("data + tcp +  ip size %d\n", headerBuffer.fTotalSize);

	return ESend(mac_dest, ETHERTYPE_IP, &headerBuffer);
	// send the packet
}


int ESend(mac_addr_t destination, uint16_t protocol,struct ChainBuffer *buffer)
{
//	TRACE(("EthernetService::Send(to: %012llx, proto: 0x%hx, %lu bytes)\n",
//		destination.ToUInt64(), protocol, (buffer ? buffer->TotalSize() : 0)));
	if (!buffer)
	{
		TRACE("ESend !buffer\n");
		return -1;
	}

	// data too long?
	size_t dataSize = buffer->fTotalSize;
	if (dataSize > ETHER_MAX_TRANSFER_UNIT)
	{
		TRACE("dataSize > ETHER_MAX_TRANSFER_UNIT\n");
		return -1;
	}

	netif_t *netif = netif_findbyname ("eth0");

	// prepend ethernet header
	packet_t header;
	struct ChainBuffer headerBuffer;
	ChainBuffer(&headerBuffer,&header, sizeof(header), buffer,false);

	memcpy (&header.mac_source, netif->dev->dev_addr, 6);
	memcpy (&header.mac_dest, destination, 6);
	header.type = NET_PACKET_TYPE_IPV4;

	// flatten
	int totalSize = headerBuffer.fTotalSize;

	char fSendBuffer[4096];
	memset(fSendBuffer,0,4096);
	Flatten(&headerBuffer,fSendBuffer);

	// pad data, if necessary
	if (dataSize < ETHER_MIN_TRANSFER_UNIT)
	{
		size_t paddingSize = ETHER_MIN_TRANSFER_UNIT - dataSize;
		memset((uint8_t*)fSendBuffer + totalSize, 0, paddingSize);
		totalSize += paddingSize;
	}

	int bytesSent = net_packet_send2(netif,fSendBuffer,totalSize);
	if (bytesSent < 0)
	{
		TRACE("bytesSent < 0\n");
		return -1;
	}
	if (bytesSent != (ssize_t)totalSize)
	{
		TRACE("(bytesSent != (ssize_t)totalSize) %d %d\n",bytesSent,totalSize);
		return -1;
	}

	return 0;
}

int SSend(struct proto_tcp_conn_context *context,struct TCPPacket* packet, bool enqueue)
{
	netif_t *netif = netif_findbyname ("eth0");

	struct ChainBuffer buffer;
	ChainBuffer(&buffer,(void*)packet->fData, packet->fSize,NULL,false);

//	TRACE("seq %d ack %d  packet->fSize %d\n",packet->fSequenceNumber, context->ack, packet->fSize);

	int error =  Send(netif,context->port_source, context->ip_dest, context->port_dest,
		packet->fSequenceNumber, context->ack, packet->fFlags,
		WindowSize(), &buffer);
	if (error != 0)
	{
		TRACE("Send return %d",error);
		return error;
	}
	if (packet->fSequenceNumber == context->seq)
		context->seq += packet->fSize;

	if (enqueue)
		_EnqueueOutgoingPacket(context,packet);

	return 0;
}


int
_ResendQueue(struct proto_tcp_conn_context *context)
{
	netif_t *netif = netif_findbyname ("eth0");
	TRACE("resending queue\n");
	struct TCPPacket* packet;
	for (packet = context->fFirstSentPacket; packet != NULL;
			packet = packet->fNext) {

		struct ChainBuffer buffer;
		ChainBuffer(&buffer,(void*)packet->fData, packet->fSize,NULL,false);


		int error = Send(netif,context->port_source, context->ip_dest, context->port_dest,
			packet->fSequenceNumber, context->ack, packet->fFlags,
			WindowSize(), &buffer);
		if (error != 0)
			return error;
	}
	return 0;
}


int
_Ack(struct proto_tcp_conn_context *context)
{
	struct TCPPacket* packet = NEW( sizeof( struct TCPPacket));
	if (packet == NULL)
		return -1;

	packet->fNext = NULL;

	int error = SetTo(packet,NULL, 0, context->ip_source, context->port_source, context->ip_dest,
			context->port_dest, context->seq, context->ack, TCP_ACK);
	if (error != 0) {
		DEL (packet);
		return error;
	}

	error = SSend(context,packet, false);
	DEL (packet);
	if (error != 0)
		return error;
	return 0;
}


int _WaitForState(struct proto_tcp_conn_context *conn,enum TCPSocketState state, long timeout)
{
	long startTime = GetTickCount();
	do
	{
		if (conn->fState == state)
			return 0;
	} while (GetTickCount() - startTime < timeout);
	return timeout == 0 ? -1 : -1;
}

unsigned short _ChecksumBuffer(struct ChainBuffer* buffer, net_ipv4 source,net_ipv4 destination, unsigned short length)
{
	struct pseudo_header header = {
		(source),
		(destination),
		0,
		IPPROTO_TCP,
		htons(length)
	};

	struct ChainBuffer headerBuffer;
	ChainBuffer(&headerBuffer,&header, sizeof(header), buffer,false);

	unsigned short checksum = ip_checksum(&headerBuffer);
	DetachNext(&headerBuffer);
	return checksum;
}

unsigned short
_ChecksumData(const void* data, unsigned short length, net_ipv4 source,
		net_ipv4 destination)
{
	struct ChainBuffer buffer;
	ChainBuffer(&buffer,(void*)data, length,NULL,false);
	return _ChecksumBuffer(&buffer, source, destination, length);
}


struct proto_tcp_conn_t*
_FindSocket(net_ipv4 address, unsigned short port)
{

	proto_tcp_conn_t *conn = NULL;
	for (conn = proto_tcp_conn_list.next; conn != &proto_tcp_conn_list; conn = conn->next)
	{
	
		if (conn->ip_source == address && conn->port_source == (port))
			return conn;
	}

	return NULL;
}


int Read(proto_tcp_conn_t *conn,void* buffer, int bufferSize, int* bytesRead,
	long timeout)
{
	if (bytesRead == NULL)
		return -1;

	while(conn->checkedask != conn->seq);

	*bytesRead = 0;
	struct TCPPacket* packet = NULL;

	long startTime = GetTickCount();
	do {
		//_ResendQueue(conn);
		packet = _PeekPacket(conn);

		if (packet == NULL && conn->fRemoteState != TCP_SOCKET_STATE_OPEN)
			return -1;
		if (packet == NULL && timeout > 0LL)
			_Ack(conn);
	} while (packet == NULL && GetTickCount() - startTime < timeout);

	if (packet == NULL) {
#ifdef TRACE_TCP_QUEUE
		_DumpQueue();
#endif
		return (timeout == 0) ? -1 : -1;
	}
	uint32_t packetOffset = conn->fNextSequence - packet->fSequenceNumber;
	int readBytes = packet->fSize - packetOffset;
	if (readBytes > bufferSize)
		readBytes = bufferSize;
	if (buffer != NULL)
		memcpy(buffer, (uint8_t*)packet->fData + packetOffset, readBytes);
	*bytesRead = readBytes;
	if (!ProvidesSequenceNumber(packet,conn->fNextSequence + readBytes)) {
		_DequeuePacket(conn);
		DEL (packet);
		packet = NULL;
	}
	conn->fNextSequence += readBytes;
	if (packet == NULL && *bytesRead < bufferSize) {
		do {
			if (buffer != NULL)
				buffer = (uint8_t*)buffer + readBytes;
			bufferSize -= readBytes;
			packet = _PeekPacket(conn);
			if (packet == NULL && conn->fRemoteState != TCP_SOCKET_STATE_OPEN)
				break;
			readBytes = 0;
			if (packet == NULL) {
				_Ack(conn);
				continue;
			}
			readBytes = packet->fSize;
			if (readBytes > bufferSize)
				readBytes = bufferSize;
			if (buffer != NULL)
				memcpy(buffer, packet->fData, readBytes);
			*bytesRead += readBytes;
			if (readBytes == packet->fSize) {
				_DequeuePacket(conn);
				DEL (packet);
			}
			conn->fNextSequence += readBytes;
		} while (readBytes < bufferSize &&
			GetTickCount() - startTime < timeout);

#ifdef TRACE_TCP_QUEUE
		if (readBytes < bufferSize) {
			TRACE_QUEUE("TCP: Unable to deliver more data!\n");
			_DumpQueue();
		}
#endif
	}

	return *bytesRead;
}

void HandleIPPacket(net_ipv4 sourceIP,
		net_ipv4 destinationIP, const void* data, size_t size)
{
	if (data == NULL || size < sizeof(proto_tcp_t))
		return;

	const proto_tcp_t* header = (const proto_tcp_t*)data;

/*	TRACE("TCPService::HandleIPPacket(): source = %d, "
		"destination = %d, %d - %d bytes\n", sourceIP, (destinationIP),
		size, sizeof(proto_tcp_t));
*/

	uint16_t chksum = _ChecksumData(data, size, sourceIP, destinationIP);
	if (chksum != 0) {
		TRACE_CHECKSUM("TCPService::HandleIPPacket(): invalid checksum "
			"(%04x vs. %04x), padding %lu\n",
			header->checksum, chksum, size % 2);
		return;
	}

	uint16_t source = ntohs(header->port_source);
	uint16_t destination = ntohs(header->port_dest);
	uint32_t sequenceNumber = ntohl(header->seq);
	uint32_t ackedNumber = ntohl(header->ack);
/*	TRACE("source = %d, dest = %d, seq = %d, ack = %d, size = %d, "
		"flags %s %s %s %s\n", source, destination, sequenceNumber,
		ackedNumber, size,
		(header->flags & TCP_ACK) != 0 ? "ACK" : "",
		(header->flags & TCP_SYN) != 0 ? "SYN" : "",
		(header->flags & TCP_FIN) != 0 ? "FIN" : "",
		(header->flags & TCP_RST) != 0 ? "RST" : "");*/
	if (header->data_offset > 5) {
		uint8_t* option = (uint8_t*)data + sizeof(proto_tcp_t);
		while ((uint32_t*)option < (uint32_t*)data + header->data_offset) {
			uint8_t optionKind = option[0];
			if (optionKind == 0)
				break;
			uint8_t optionLength = 1;
			if (optionKind > 1) {
				optionLength = option[1];
		//		TRACE("\tTCP option kind %u, length %u\n",
		//			optionKind, optionLength);
		//		if (optionKind == 2)
		//			TRACE("\tTCP MSS = %04hu\n", *(uint16_t*)&option[2]);
			}
			option += optionLength;
		}
	}

	struct proto_tcp_conn_context* socket = _FindSocket(destinationIP, destination);
	if (socket == NULL) {
		// TODO If SYN, answer with RST?
		TRACE("TCPService::HandleIPPacket(): no socket\n");
		return;
	}

	if ((header->flags & TCP_ACK) != 0) {
		Acknowledge(socket,ackedNumber);
	}

	struct TCPPacket* packet = NEW( sizeof(struct TCPPacket));
	if (packet == NULL)
		return;

	packet->fNext = NULL;

	socket->checkedask = ackedNumber;

	int error = SetTo(packet,(uint32_t*)data + header->data_offset,
		size - header->data_offset * 4, sourceIP, source, destinationIP,
		destination, sequenceNumber, ackedNumber, header->flags);
	if (error == 0)
		ProcessPacket(socket,packet);
	else
		DEL (packet);
}



void Iterator(struct Iterator *iter,struct ChainBuffer *buffer)
{
	iter->fBuffer = buffer;
	iter->fOffset = -1;
	_Next(iter);
}

bool HasNext(struct Iterator *iter)
{
	return iter->fBuffer;
}

uint16_t Next(struct Iterator *iter)
{
	uint16_t byte = _NextByte(iter);
	return (byte << 8) | _NextByte(iter);
}

void _Next(struct Iterator *iter)
{
	while (iter->fBuffer) {
		iter->fOffset++;
		if (iter->fOffset < (int)iter->fBuffer->fSize)
			break;

		iter->fOffset = -1;
		iter->fBuffer = iter->fBuffer->fNext;
	}
}

uint8_t _NextByte(struct Iterator *iter)
{
	uint8_t byte = (iter->fBuffer ? ((uint8_t*)iter->fBuffer->fData)[iter->fOffset] : 0);
	_Next(iter);
	return byte;
}

uint16_t ip_checksum(struct ChainBuffer *buffer)
{
	struct Iterator it;
	Iterator(&it,buffer);

	uint32_t checksum = 0;
	while (HasNext(&it))
	{
		checksum += Next(&it);
		while (checksum >> 16)
			checksum = (checksum & 0xffff) + (checksum >> 16);
	}

	return ~checksum;
}

uint16_t _Checksum(struct proto_ip_t *header)
{
	struct ChainBuffer buffer;
	ChainBuffer(&buffer,(void*)header, header->head_len * 4,NULL,false);
	return ip_checksum(&buffer);
}

