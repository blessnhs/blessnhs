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
#include "ndp.h"
#include "tcp.h"
#include "tun.h"
#include "socket.h"
#include "../Synchronization.h"
#include "../errno.h"
#include "fd.h"
#include "file.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

/* Mutex for socket function */
MUTEX_CREATE (mutex_tcp6_accept);
MUTEX_CREATE (mutex_tcp6_read_cache);

proto_tcp6_conn_t proto_tcp6_conn_list;
proto_tcp6_backlog_t proto_tcp6_backlog_list;

extern netif_t netif_list;

static unsigned short proto_tcp6_dup;
static unsigned proto_tcp6_seq;
static proto_ipv6_t proto_ipv6_prealloc;
static proto_tcp_t proto_tcp_prealloc;
static packet_t packet_prealloc;
static char buf_tcp_prealloc[NET_PACKET_MTU + sizeof (proto_tcp_t) + 1];

/* prototype */
proto_tcp6_conn_t *net_proto_tcp6_conn_check (net_ipv6 ip_source, net_port port_source, net_ipv6 ip_dest, net_port port_dest, unsigned char *ret);
int net_proto_tcp6_conn_add ();
int net_proto_tcp6_conn_set (proto_tcp6_conn_t *conn, netif_t *eth, net_port port_source, net_ipv6 ip_dest, net_port port_dest);
int net_proto_tcp6_conn_estabilish (proto_tcp6_conn_t *conn, netif_t *eth, net_ipv6 ip_dest, net_port port_dest);
int net_proto_tcp6_conn_estabilish_reply (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp6_read_cache (proto_tcp6_conn_t *conn, char *data, unsigned len);
int net_proto_tcp6_read_ok (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old, unsigned len);
int net_proto_tcp6_write (netif_t *eth, net_ipv6 dest, proto_tcp_t *tcp, char *data, unsigned len);
int net_proto_tcp6_write_data (proto_tcp6_conn_t *conn, char *data, unsigned len);
int net_proto_tcp6_write_data_ok (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp6_conn_disconnected (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old);
int net_proto_tcp6_conn_close (proto_tcp6_conn_t *conn);
unsigned net_proto_tcp6_conn_del (proto_tcp6_conn_t *conn);
proto_tcp6_conn_t *net_proto_tcp6_conn_find (int fd);
int net_proto_tcp6_backlog_add (proto_tcp6_conn_t *conn, net_ipv6 ip, net_port port, unsigned seq);
int net_proto_tcp6_conn_invite (proto_tcp6_conn_t *conn, net_ipv6 ip, net_port port, unsigned seq);

/** TCPv6 protocol
 *  User-friendly socket functions
 */
int net_proto_tcp6_socket (fd_t *fd)
{
	return net_proto_tcp6_conn_add (fd);
}

int net_proto_tcp6_connect (int fd, sockaddr_in6 *addr)
{
	int ret = -1;

	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		ret = net_proto_tcp6_conn_estabilish (conn, netif, addr->sin6_addr, addr->sin6_port);

		unsigned long stime = GetTickCount();

		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			if (!ret)
				return -1;

			while (1) {
				Schedule ();

				/* timeout for 3s */
				if ((stime+3000) < GetTickCount())
					return -1;

				if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISHED)
					break;

				/* when connection cant be accepted succefully first time, try it again */
				if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISHERROR) {
					ret = net_proto_tcp6_conn_estabilish (conn, netif, addr->sin6_addr, addr->sin6_port);
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

int net_proto_tcp6_send (int fd, char *msg, unsigned size)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	unsigned len = size;
	
	unsigned mtu_tcp = NET_PACKET_MTU - sizeof (proto_tcp_t) - sizeof (net_ipv6) - 16;

check:
	if (size > mtu_tcp) {
		int r = net_proto_tcp_send (fd, msg, mtu_tcp);

		if (r <= 0)
			return r;
		
		msg += mtu_tcp;
		size -= mtu_tcp;
		
		goto check;
	}
	
	int ret = net_proto_tcp6_write_data (conn, msg, size);

	if (ret) {
		unsigned long stime = GetTickCount();

		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			while (conn->state != PROTO_TCP_CONN_STATE_READY) {
				Schedule ();

				/* timeout for 350ms */
				if ((stime+350) < GetTickCount())
					return -1;
			}
		} else {
		/* non-blocking mode */
		/* TODO: ? */
		//#ifdef TEST
			while (conn->state != PROTO_TCP_CONN_STATE_READY) {
				Schedule ();

				/* timeout for 100ms */
				if ((stime+100) < GetTickCount())
					return -1;
			}
		//#endif
		}
	}

	return len;
}

int net_proto_tcp6_recv (int fd, char *msg, unsigned size)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -3;

	int ret = 0;

	//kPrintf ("recv () - %d\n", fd);

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		unsigned long stime = GetTickCount();

		while (!conn->len/* || conn->state != PROTO_TCP_CONN_STATE_READY*/) {
			if ((stime+10000) < GetTickCount())
				return -1;

			if (conn->state == PROTO_TCP_CONN_STATE_CLOSE) {
				if (net_proto_tcp6_conn_del (conn));
					return -1;
			}

			Schedule ();
		}
	} else {
		if (conn->state == PROTO_TCP_CONN_STATE_CLOSE) {
			if (net_proto_tcp6_conn_del (conn));
				return -1;
		}

		if (!conn->len)
			return 0;
	}

	if ((conn->len-conn->offset) > size) {
		memcpy (msg, conn->data+conn->offset, size);
		//Printf ("msg: %d %d\n", conn->offset, size);
		conn->offset += size;
	
		ret = size;
	} else {
		ret = conn->len-conn->offset;
	
		memcpy (msg, conn->data+conn->offset, conn->len-conn->offset);
		//Printf ("msg2: %d %d\n", conn->offset, size);
		conn->len = 0;
		conn->offset = 0;
	
		DEL (conn->data);
	}

	//kPrintf ("recv () - %d -- DATA: %d\n", fd, ret);

	return ret;
}

int net_proto_tcp6_close (int fd)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	if (conn->state == PROTO_TCP_CONN_STATE_DISCONNECTED)
		return 0;

	int ret = net_proto_tcp6_conn_close (conn);

	return ret;
}

int net_proto_tcp6_fcntl (int fd, int cmd, long arg)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

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

int net_proto_tcp6_bind (int fd, sockaddr_in6 *addr, socklen_t len)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		net_proto_tcp6_conn_set (conn, netif, addr->sin6_port, 0, 0);
		conn->bind = 1;
		return 0;
	}

	return -1;
}

int net_proto_tcp6_listen (int fd, int backlog)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	return 0;
}

int net_proto_tcp6_accept (int fd, sockaddr_in6 *addr, socklen_t *addrlen)
{
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	unsigned i = 0;

	proto_tcp6_backlog_t *backlog = 0;

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		while (!i) {
			for (backlog = proto_tcp6_backlog_list.next; backlog != &proto_tcp6_backlog_list; backlog = backlog->next) {
				if (backlog->conn == conn) {
					i = 1;
					break;
				}
			}

			Schedule ();
		}
	} else {
	/* non-blocking mode */
		for (backlog = proto_tcp6_backlog_list.next; backlog != &proto_tcp6_backlog_list; backlog = backlog->next) {
			if (backlog->conn == conn) {
				i = 1;
				break;
			}
		}
	}

	//Printf ("accept ()\n");

	if (!i)
		return -1;

	net_ipv6 ip;
	memcpy (ip, backlog->ip, sizeof (net_ipv6));

	net_port port = backlog->port;
	unsigned seq = backlog->seq;

	int ret = net_proto_tcp6_conn_invite (conn, ip, port, seq);

	/* remove from queue accepted connection */
	backlog->next->prev = backlog->prev;
	backlog->prev->next = backlog->next;

	DEL (backlog);

	if (ret < 1)
		return ret;

	fd_t *fd_new = fd_create (FD_SOCK);
	
	if (!fd_new)
		return 0;
	
	net_proto_tcp6_conn_add (fd_new);

	proto_tcp6_conn_t *conn_new = net_proto_tcp6_conn_find (fd_new->id);

	if (!conn_new)
		return -1;

	net_proto_tcp6_conn_set (conn_new, conn->netif, conn->port_source, conn->netif->ipv6, port);

	memcpy (conn_new->ip_dest, ip, sizeof (net_ipv6));

	memcpy (addr->sin6_addr, ip, sizeof (net_ipv6));
	addr->sin6_port = port;
	addr->sin6_family = AF_INET6;

	return fd_new->id;
}

int net_proto_tcp6_select (int readfd, int writefd, int exceptfd)
{
	int fd = -1;
  
	if (readfd)
		fd = readfd;
	else if (writefd)
		fd = writefd;
	else if (exceptfd)
		fd = exceptfd;

	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_find (fd);

	if (!conn)
		return -1;

	/* check for incoming connections */
	if (conn->bind) {
		proto_tcp6_backlog_t *backlog = 0;

		for (backlog = proto_tcp6_backlog_list.next; backlog != &proto_tcp6_backlog_list; backlog = backlog->next) {
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
unsigned net_proto_tcp6_handler (packet_t *packet, proto_ipv6_t *ip, char *buf, unsigned len, unsigned flags)
{
	proto_tcp_t *tcp = (proto_tcp_t *) buf;

	unsigned char ret = 0;

	/* First check ip address and ports, that we want this data */
	proto_tcp6_conn_t *conn = net_proto_tcp6_conn_check (ip->ip_dest, tcp->port_dest, ip->ip_source, tcp->port_source, &ret);

	//Printf ("hmm: %d - %d - %d\n", len, swap16 (tcp->port_dest), ret);

	if (!conn && ret == 0)
		return 1;

	//Printf ("tcp6->flags: 0x%x, ret: %d, conn: 0x%x, fd: %d\n", tcp->flags, ret, conn, conn->fd);

	/* connection from client before accept () */
	if (ret == 1) {
		/* client want connect to our server */
		if (tcp->flags == 0x02) {
			Lock (&mutex_tcp6_accept);

			unsigned ret = net_proto_tcp6_backlog_add (conn, ip->ip_source, tcp->port_source, tcp->seq);

			Unlock (&mutex_tcp6_accept);

			return ret; 
		}

		return 1;
	}

	unsigned data_cache = 0;

	/* data received */
	if (tcp->flags & 0x08) {
		/* needed for calculate real offset from 4bit number */
		unsigned offset = tcp->data_offset * 4;
		unsigned size = len - offset;

		//kPrintf (">>>> %d : %d : %d\n", offset, size, len);
		//Lock (&mutex_tcp6_read_cache);
		net_proto_tcp6_read_cache (conn, buf+offset, size);
		//Unlock (&mutex_tcp6_read_cache);
		/* send ack */
		net_proto_tcp6_read_ok (conn, ip, tcp, size);

		data_cache = 1;
	}

	/* sended data was delivered succefully */
	if (tcp->flags & 0x10) {
		/* HACK: It is interesting, that no push flag, and there could be some data to read */

		/* needed for calculate real offset from 4bit number */
		unsigned offset = tcp->data_offset * 4;
		unsigned size = len - offset;

		/* there are data for read */
		if (size) {
			/* data was cached, so no need cache it again */
			if (!data_cache) {
				//kPrintf (">>>>2 %d : %d : %d\n", offset, size, len);
				Lock (&mutex_tcp6_read_cache);
				net_proto_tcp6_read_cache (conn, buf+offset, size);
				Unlock (&mutex_tcp6_read_cache);
				/* send ack */
				net_proto_tcp6_read_ok (conn, ip, tcp, size);
			}
		} else {
			/* when data are'nt available, lets normal work - acknowledgement */
			if (conn->state == PROTO_TCP_CONN_STATE_ESTABILISH) {
				conn->state = PROTO_TCP_CONN_STATE_ESTABILISHERROR;
				net_proto_tcp6_write_data_ok (conn, ip, tcp);
			} else
				net_proto_tcp6_write_data_ok (conn, ip, tcp);
		}
	}

	/* connection estabilish respond */
	if (tcp->flags == 0x12) {
		return net_proto_tcp6_conn_estabilish_reply (conn, ip, tcp);
	}

	/* another side close connection */
	if (tcp->flags & 0x01) {
		Printf("TCP -> fd %d connection closed by remote host\n", conn->fd);
		net_proto_tcp6_conn_disconnected (conn, ip, tcp);			// FIXME: problem in server with hangind the whole program
	}

	/* another side close connection */
	if (tcp->flags & 0x04) {
		Printf("TCP -> fd %d connection reseted by peer\n", conn->fd);
		net_proto_tcp6_conn_disconnected (conn, ip, tcp);
	}

	return 1;
}

int net_proto_tcp6_read_cache (proto_tcp6_conn_t *conn, char *data, unsigned len)
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

	//kPrintf ("DATA: %d - #'%s'#\n", len, data);
	
	conn->len += len;

	conn->data[conn->len] = '\0';

	return conn->len;
}

int net_proto_tcp6_read_ok (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old, unsigned len)
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

	int ret = net_proto_tcp6_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	/*if (proto_tcp6_dup == tcp_old->checksum) {
		Printf ("Duplicated packet :)");
		Sleep (150);
	}*/

	if (ret) {
		//Printf ("read_ok: seq: 0x%x | ack: 0x%x\n", conn->seq, conn->ack);

		conn->seq = tcp->ack;
		conn->ack = tcp->seq;

		//Printf ("read_ok2: seq: 0x%x | ack: 0x%x\n", conn->seq, conn->ack);

		proto_tcp6_dup = tcp_old->checksum;

		conn->cross = 1;
	}

	DEL (tcp);

	return ret;
}

int net_proto_tcp6_write (netif_t *eth, net_ipv6 dest, proto_tcp_t *tcp, char *data, unsigned len)
{
	if (!eth || !tcp) {
		Printf ("!eth || !tcp\n");
		return 0;
	}
	if (len)
		if (!data) {
			Printf ("!data\n");
			return 0;
		}
	mac_addr_t mac_dest;

	if (!tun6_addr_get ()) {
		unsigned get = ndp_cache_get (dest, &mac_dest);
	
		if (!get) {
			ndp_send_nbrequest (eth, dest);
	
			unsigned i = 0;
			/* 200ms for waiting on NDP reply */
			while (i < 200) {
				get = ndp_cache_get (dest, &mac_dest);
	
				if (get)
					break;
	
				/* TODO: make better waiting for NDP reply */
				Sleep (1);
	
				Schedule ();
		
				i ++;
			}
	
			if (!get) {
				Printf ("!get\n");
				return 0;
			}
		}
	} else {
		/* IPv6 tunnel stuff */
		net_ipv4 tunnel = tun6_addr_get ();
		unsigned get = arp_cache_get (tunnel, &mac_dest);
	
		if (!get) {
			arp_send_request (eth, tunnel);
	
			unsigned i = 0;
			/* 100ms for waiting on ARP reply */
			while (i < 100) {
				get = arp_cache_get (tunnel, &mac_dest);
	
				if (get)
					break;
	
				/* TODO: make better waiting for ARP reply */
				Sleep (1);
	
				Schedule ();
		
				i ++;
			}
	
			if (!get) {
				Printf ("!get2\n");
				return 0;
			}
		}
	}

	/* packet header */
	packet_t *packet = &packet_prealloc;

	if (!packet) {
		Printf ("!packet\n");
		return 0;
	}

	memcpy (&packet->mac_source, eth->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	unsigned l = (tcp->data_offset*4);

	/* ip layer */
	proto_ipv6_t *ip = &proto_ipv6_prealloc;

	if (!ip) {
		Printf ("!ip\n");
		return 0;
	}

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (l+len);
	ip->nhead = NET_PROTO_IP_TYPE_TCP6;
	ip->hop = 0xff;
	memcpy (ip->ip_source, (void *) eth->ipv6, sizeof (net_ipv6));
	memcpy (ip->ip_dest, (void *) dest, sizeof (net_ipv6));

	if (len+l > NET_PACKET_MTU + sizeof (proto_tcp_t)) {
		Printf("TCP -> data lenght is exceed: %d/%d bytes", len+l, NET_PACKET_MTU + sizeof (proto_tcp_t) + 1);
		return 0;
	}
	
	char *buf_tcp = (char *) &buf_tcp_prealloc;

	if (!buf_tcp) {
		Printf ("!buf_tcp\n");
		return 0;
	}

	memcpy (buf_tcp, (char *) tcp, l);

	if (len)
		memcpy (buf_tcp+l, data, len);

	buf_tcp[l+len] = '\0';

	/* calculate checksum and put it to tcp header */
	proto_tcp_t *tcp_ = (proto_tcp_t *) buf_tcp;

	unsigned ret = 0;

	tcp_->checksum = checksum16_ipv6 (eth->ipv6, dest, buf_tcp, l+len, NET_PROTO_IP_TYPE_TCP6);

	/* send tcp header+data to ip layer */
	if (!tun6_addr_get ())
		ret = net_proto_ipv6_send (eth, packet, ip, (char *) buf_tcp, l+len);
	else
		ret = net_proto_tun6_send (eth, packet, ip, (char *) buf_tcp, l+len);

	return ret;
}

int net_proto_tcp6_write_data (proto_tcp6_conn_t *conn, char *data, unsigned len)
{
	if (!conn || !len || !data)
		return 0;

	proto_tcp_t *tcp = &proto_tcp_prealloc;

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source;
	tcp->port_dest = conn->port_dest;

	unsigned seq = swap32 (proto_tcp6_seq);
	seq ++;

	//tcp->ack = swap32 (seq);

	proto_tcp6_seq = swap32 (seq);

	if (!conn->cross) {
		tcp->seq = conn->seq;
		tcp->ack = conn->ack;
	} else {
		tcp->seq = conn->ack;
		tcp->ack = conn->seq;

		conn->cross = 0;
	}

	//Printf ("TCP -> seq: 0x%x | ack: 0x%x | proto_seq: 0x%x\n", tcp->seq, conn->ack, proto_tcp6_seq);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x18;
	tcp->window = swap16 (32768);
	tcp->checksum = 0;

	int ret = net_proto_tcp6_write (conn->netif, conn->ip_dest, tcp, data, len);

	if (ret)
		conn->state = PROTO_TCP_CONN_STATE_WAIT;

	return ret;
}

int net_proto_tcp6_write_data_ok (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	/* cross - because we change now sides :) */
	conn->seq = tcp_old->ack;
	conn->ack = tcp_old->seq;

	//Printf ("data_ok: seq: 0x%x | ack: 0x%x\n", tcp_old->ack, tcp_old->seq);

	conn->state = PROTO_TCP_CONN_STATE_READY;

	return 1;
}

/* try connect to server */
int net_proto_tcp6_conn_estabilish (proto_tcp6_conn_t *conn, netif_t *eth, net_ipv6 ip_dest, net_port port_dest)
{
	if (!conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = swap16 (netif_port_get ()); // increase client port too
	tcp->port_dest = port_dest;

	/* setup new connection */
	net_proto_tcp6_conn_set (conn, eth, tcp->port_source, ip_dest, tcp->port_dest);

	tcp->seq = conn->seq;
	tcp->ack = conn->ack;

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x02;
	tcp->window = swap16 (32792);
	tcp->checksum = 0;

	int ret = net_proto_tcp6_write (eth, ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (!ret) {
		net_proto_tcp6_conn_del (conn);
		return 0;
	}

	return 1;
}

int net_proto_tcp6_conn_estabilish_reply (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old)
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

	int ret = net_proto_tcp6_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	if (ret) {
		conn->seq = tcp->seq;
		conn->ack = tcp->ack;
		conn->state = PROTO_TCP_CONN_STATE_ESTABILISHED;
	}

	//Printf( "TCP -> fd %d connected to server succefully\n", conn->fd);

	DEL (tcp);

	return ret;
}

int net_proto_tcp6_conn_disconnected (proto_tcp6_conn_t *conn, proto_ipv6_t *ip_old, proto_tcp_t *tcp_old)
{
	if (!ip_old || !tcp_old || !conn)
		return 0;

	if (conn->state == PROTO_TCP_CONN_STATE_CLOSE)
		return net_proto_tcp6_conn_del (conn);

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

	int ret = net_proto_tcp6_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (ret)
		conn->state = PROTO_TCP_CONN_STATE_CLOSE; //ret = net_proto_tcp6_conn_del (conn);
	else
		return -1;

	return ret;
}

int net_proto_tcp6_conn_close (proto_tcp6_conn_t *conn)
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

	int ret = net_proto_tcp6_write (conn->netif, conn->ip_dest, tcp, 0, 0);

	DEL (tcp);

	if (ret) {
		conn->state = PROTO_TCP_CONN_STATE_CLOSE;
	}

	return ret;
}

/* client wants to connect */
net_proto_tcp6_conn_invite (proto_tcp6_conn_t *conn, net_ipv6 ip, net_port port, unsigned seq)
{	
	if (!conn)
		return 0;

	proto_tcp_t *tcp = (proto_tcp_t *) NEW (sizeof (proto_tcp_t));

	if (!tcp)
		return 0;

	tcp->port_source = conn->port_source;
	tcp->port_dest = port;

	tcp->seq = proto_tcp6_seq ++;
	tcp->ack = seq + swap32 (1);

	tcp->res = 0;
	tcp->data_offset = 5;
	tcp->flags = 0x12;
	tcp->window = swap16 (5816);
	tcp->checksum = 0;

	int ret = net_proto_tcp6_write (conn->netif, ip, tcp, 0, 0);
	
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
int net_proto_tcp6_conn_add (fd_t *fd)
{
	proto_tcp6_conn_t *conn;

	/* alloc and init context */
	conn = (proto_tcp6_conn_t *) NEW (sizeof (proto_tcp6_conn_t));

	if (!conn) {
		errno_set (ENOMEM);
		return -1;
	}

	memset (conn, 0, sizeof (proto_tcp6_conn_t));

	conn->flags = 0;
	conn->bind = 0;

	conn->fd = fd->id;

	conn->cross = 0;

	/* add into list */
	conn->next = &proto_tcp6_conn_list;
	conn->prev = proto_tcp6_conn_list.prev;
	conn->prev->next = conn;
	conn->next->prev = conn;

	return conn->fd;
}

/* Setup new connection */
int net_proto_tcp6_conn_set (proto_tcp6_conn_t *conn, netif_t *eth, net_port port_source, net_ipv6 ip_dest, net_port port_dest)
{
	if (!conn || !eth)
		return 0;

	memcpy (conn->ip_source, eth->ipv6, sizeof (net_ipv6));
	memcpy (conn->ip_dest, ip_dest, sizeof (net_ipv6));

	conn->port_source = port_source;
	conn->port_dest = port_dest;

	proto_tcp6_seq += swap32 (1);

	conn->seq = proto_tcp6_seq ++;
	conn->ack = 0;

	conn->state = PROTO_TCP_CONN_STATE_ESTABILISH;

	conn->netif = eth;

	conn->offset = 0;
	conn->len = 0;
	conn->data = 0;

	return 1;
}

/* Delete existing connection from list */
unsigned net_proto_tcp6_conn_del (proto_tcp6_conn_t *conn)
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

proto_tcp6_conn_t *net_proto_tcp6_conn_check (net_ipv6 ip_source, net_port port_source, net_ipv6 ip_dest, net_port port_dest, unsigned char *ret)
{
	*ret = 0;
	proto_tcp6_conn_t *conn = NULL;
	proto_tcp6_conn_t *conn_ret = NULL;
	//Printf ("-------------------------\n");
	for (conn = proto_tcp6_conn_list.next; conn != &proto_tcp6_conn_list; conn = conn->next) {
		/*Printf ("-> ");
		net_proto_ipv6_print (conn->ip_source);
		Printf (" / ");
		net_proto_ipv6_print (ip_source);
		Printf ("\n2> ");
		net_proto_ipv6_print (conn->ip_dest);
		Printf (" / ");
		net_proto_ipv6_print (ip_dest);
		Printf ("\nporty: %d / %d\n", swap16 (conn->port_source), swap16 (port_source));
		Printf ("porty2: %d / %d\n", swap16 (conn->port_dest), swap16 (port_dest));*/

		if (net_proto_ipv6_cmp (conn->ip_source, ip_source) && conn->port_source == port_source) {
			if (net_proto_ipv6_cmp (conn->ip_dest, ip_dest) && conn->port_dest == port_dest) {
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

			for (conn = proto_tcp6_conn_list.next; conn != &proto_tcp6_conn_list; conn = conn->next) {
				if (conn->bind) {
					if (net_proto_ipv6_cmp (conn->ip_source, ip_source) && conn->port_source == port_source)
						conn_ret = conn;
				}
			}
		}
	
	return conn_ret;
}

proto_tcp6_conn_t *net_proto_tcp6_conn_find (int fd)
{
	proto_tcp6_conn_t *conn = NULL;
	for (conn = proto_tcp6_conn_list.next; conn != &proto_tcp6_conn_list; conn = conn->next) {
		if (conn->fd == fd)
			return conn;
	}
	
	return 0;
}

/* Create new TCP backlog stamp */
int net_proto_tcp6_backlog_add (proto_tcp6_conn_t *conn, net_ipv6 ip, net_port port, unsigned seq)
{
	if (!conn)
		return 0;

	proto_tcp6_backlog_t *backlog;

	/* alloc and init context */
	backlog = (proto_tcp6_backlog_t *) NEW (sizeof (proto_tcp6_backlog_t));

	if (!backlog)
		return 0;

	backlog->conn = conn;
	memcpy (backlog->ip, ip, sizeof (net_ipv6));
	backlog->port = port;
	backlog->seq = seq;

	/* add into list */
	backlog->next = &proto_tcp6_backlog_list;
	backlog->prev = proto_tcp6_backlog_list.prev;
	backlog->prev->next = backlog;
	backlog->next->prev = backlog;
	
	return conn->fd;
}

/* init of tcp protocol */
unsigned init_net_proto_tcp6 ()
{
	proto_tcp6_conn_list.next = &proto_tcp6_conn_list;
	proto_tcp6_conn_list.prev = &proto_tcp6_conn_list;

	proto_tcp6_backlog_list.next = &proto_tcp6_backlog_list;
	proto_tcp6_backlog_list.prev = &proto_tcp6_backlog_list;

	proto_tcp6_dup = 0x0;
	proto_tcp6_seq = 0x1;

	return 1;
}


