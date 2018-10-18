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
#include "ip.h"
#include "packet.h"
#include "udp.h"
#include "socket.h"
#include "fd.h"
#include "../errno.h"
#include "file.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

extern netif_t netif_list;

proto_udp_conn_t proto_udp_conn_list;

static proto_ip_t proto_ip_prealloc;
static proto_udp_t proto_udp_prealloc;
static packet_t packet_prealloc;
static char buf_udp_prealloc[NET_PACKET_MTU + sizeof (proto_udp_t) + 1];

/* prototypes */
proto_udp_conn_t *net_proto_udp_conn_check (net_ipv4 ip_source, net_port port_source, net_ipv4 ip_dest, net_port port_dest, unsigned char *ret);
int net_proto_udp_read_cache (proto_udp_conn_t *conn, char *data, unsigned len);
int net_proto_udp_write (proto_udp_conn_t *conn, char *data, unsigned len);
proto_udp_conn_t *net_proto_udp_conn_find (int fd);
int net_proto_udp_conn_set (proto_udp_conn_t *conn, netif_t *eth, net_port port_source, net_ipv4 ip_dest, net_port port_dest);
unsigned net_proto_udp_conn_del (proto_udp_conn_t *conn);
int net_proto_udp_conn_add ();

/** UDP protocol
 *  User-friendly socket functions
 */
int net_proto_udp_socket (fd_t *fd)
{
	return net_proto_udp_conn_add (fd);
}

int net_proto_udp_connect (int fd, sockaddr_in *addr)
{
	int ret = -1;

	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		ret = net_proto_udp_conn_set (conn, netif, swap16 (netif_port_get ()), addr->sin_addr, addr->sin_port);
		
		/* TODO: connect timeout */

		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			if (!ret)
				return -1;
		} else
		/* non-blocking mode */
			return -1;

		ret = 0;
	}

	return ret;
}

int net_proto_udp_send (int fd, char *msg, unsigned size)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	unsigned len = size;
	
	unsigned mtu_udp = NET_PACKET_MTU - sizeof (proto_udp_t) - sizeof (net_ipv4) - 16;

check:
	if (size > mtu_udp) {
		int r = net_proto_udp_send (fd, msg, mtu_udp);

		if (r <= 0)
			return r;
		
		msg += mtu_udp;
		size -= mtu_udp;
		
		goto check;
	}	
	
	int ret = net_proto_udp_write (conn, msg, size);

//	Printf("net_proto_udp_write ret %d\n",ret);

	if (ret) {
		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			
		} else {
		/* non-blocking mode */
		/* TODO: ? */
			
		}
	}

	return len;
}

int net_proto_udp_sendto (int fd, char *msg, unsigned size, sockaddr_in *to)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	unsigned len = size;
	
	unsigned mtu_udp = NET_PACKET_MTU - sizeof (proto_udp_t) - sizeof (net_ipv4) - 16;

check:
	if (size > mtu_udp) {
		int r = net_proto_udp_sendto (fd, msg, mtu_udp, to);

		if (r <= 0)
			return r;
		
		msg += mtu_udp;
		size -= mtu_udp;
		
		goto check;
	}

	int ret = net_proto_udp_write (conn, msg, size);

	if (ret) {
		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {

		} else {
		/* non-blocking mode */
		/* TODO: ? */
			
		}
	}

	return len;
}

extern unsigned long timer_ticks;
int net_proto_udp_recv (int fd, char *msg, unsigned size)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -3;

	if (!msg)
		return -4;

	int ret = 0;

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		unsigned long stime = GetTickCount();

		while (!conn->len) {
			if ((stime+10000) < GetTickCount())
				return 0;

			Schedule ();
		}
	} else {
		if (!conn->len)
			return 0;
	}

	if (!conn->data)
		return -5;

	if (conn->len >= size)
		return -2;

	ret = conn->len;

	if (conn->len > 0)
		memcpy (msg, conn->data, conn->len);

	conn->len = 0;

	DEL (conn->data);

	return ret;
}

int net_proto_udp_recvfrom (int fd, char *msg, unsigned size, sockaddr_in *from)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -3;

	if (!msg)
		return -4;

	int ret = 0;

	//int ip = conn->ip_dest;
	
	from->sin_addr = conn->ip_dest;
	//kPrintf ("recvfrom -> ip: 0x%x\n", from->sin_addr);

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		/* TODO: timeout */
		while (!conn->len)
			Schedule ();
	} else {
		if (!conn->len) {
			//conn->ip_dest = ip;
			return 0;
		}
	}

	//conn->ip_dest = ip;	// return old ip address back to structure

	if (!conn->data)
		return -5;

	if (conn->len >= size)
		return -2;

	ret = conn->len;

	if (conn->len > 0)
		memcpy (msg, conn->data, conn->len);

	conn->len = 0;

	DEL (conn->data);

	return ret;
}

int net_proto_udp_close (int fd)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_udp_conn_del (conn);

	return ret;
}

int net_proto_udp_fcntl (int fd, int cmd, long arg)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

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

int net_proto_udp_bind (int fd, sockaddr_in *addr, socklen_t len)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		net_proto_udp_conn_set (conn, netif, addr->sin_port, 0, 0);
		conn->bind = 1;
		return 0;
	}

	return -1;
}

/** special functions for kernel purposes **/
/* set source ip address as anycast */
int net_proto_udp_anycast (int fd)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	conn->ip_source = INADDR_ANY;

	return 0;
}

/* set source port to specified one */
int net_proto_udp_port (int fd, net_port port)
{
	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	conn->port_source = port;

	return 0;
}

int net_proto_udp_select (int readfd, int writefd, int exceptfd)
{
	int fd = -1;
  
	if (readfd)
		fd = readfd;
	else if (writefd)
		fd = writefd;
	else if (exceptfd)
		fd = exceptfd;

	proto_udp_conn_t *conn = net_proto_udp_conn_find (fd);

	if (!conn)
		return -1;

	/* New incoming connection */
	if (conn->state == PROTO_UDP_CONN_STATE_NEW) {
		conn->state = 0;
		return 1;
	}

	/* Are some data available ? */
	if (conn->len > 0)
		return 1;
	
	return 0;
}

/** UDP protocol
 *  hardcore code :P
 */

/* handler for received udp datagrams */
unsigned net_proto_udp_handler (packet_t *packet, proto_ip_t *ip, char *buf, unsigned len)
{
	unsigned char ret;
	proto_udp_t *udp = (proto_udp_t *) buf;

	/* check ip address and ports first */
	proto_udp_conn_t *conn = net_proto_udp_conn_check (ip->ip_dest, udp->port_dest, ip->ip_source, udp->port_source, &ret);

	if (!conn || ret == 0)
		return 1;

	if (ret == 1)
		conn->ip_dest = ip->ip_source;

	/* HACK: very ugly */
	conn->port_dest = udp->port_source;

	unsigned short l = htons (udp->len);

	if (l < 1)
		return 0;

	/* save data to buffer; +8 because udp packet contain 8byte header and than data */
	int ret2 = net_proto_udp_read_cache (conn, buf+8, l-8);

	if (ret2 < 1)
		return 0;

	return 1;
}

/* save received data to buffer */
int net_proto_udp_read_cache (proto_udp_conn_t *conn, char *data, unsigned len)
{
	if (!data || !conn || !len)
		return 0;

	if (!conn->len)
		conn->data = (char *) NEW (sizeof (char) * (len + 1));
	else
		conn->data = (char *) krealloc (conn->data, (sizeof (char) * (conn->len+len)));

	if (!conn->data)
		return -1;

	memcpy (conn->data+conn->len, data, len);

	conn->len += len;

	//conn->data[conn->len] = '\0';

	return conn->len;
}

int net_proto_udp_write (proto_udp_conn_t *conn, char *data, unsigned len)
{
	if (!conn || !len || !data)
	{
		Printf("net_proto_udp_write !conn || !len || !data\n ");
		return 0;
	}

	mac_addr_t mac_dest;
	unsigned get = arp_cache_get (conn->ip_dest, &mac_dest);

	if (!get)
	{
		conn->netif = netif_findbyname ("eth0");
		arp_send_request (conn->netif, conn->ip_dest);

		unsigned i = 0;
		/* 100ms for waiting on ARP reply */
		while (i < 100) {
			get = arp_cache_get (conn->ip_dest, &mac_dest);

			if (get)
				break;

			/* TODO: make better waiting for ARP reply */
			Sleep (10);

			Schedule ();
	
			i ++;
		}

		if (!get)
		{
			Printf("net_proto_udp_write !get\n ");
			return 0;
		}
	}

	/* packet header */
	packet_t *packet = (packet_t *) &packet_prealloc;

	if (!packet)
	{
		Printf("net_proto_udp_write !packet\n ");
		return 0;
	}

	memcpy (&packet->mac_source, conn->netif->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV4;

	/* ip layer */
	proto_ip_t *ip = (proto_ip_t *) &proto_ip_prealloc;

	if (!ip)
	{
		Printf("net_proto_udp_write !ip\n ");
		return 0;
	}

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 4;
	ip->head_len = 5;

	ip->flags = 0;
	ip->frag = 0;
	ip->ttl = 64;
	ip->checksum = 0;
	ip->proto = NET_PROTO_IP_TYPE_UDP;
	ip->ip_source = conn->ip_source;
	ip->ip_dest = conn->ip_dest;


	proto_udp_t *udp = (proto_udp_t *) &proto_udp_prealloc;

	if (!udp)
	{
		Printf("net_proto_udp_write !udp\n ");
		return 0;
	}

	udp->port_source = conn->port_source;
	udp->port_dest = conn->port_dest;
	udp->len = swap16 (8 + len);
	udp->checksum = 0;

	unsigned l = (ip->head_len * 4);

	/* calculate total length of packet (udp/ip) */
	ip->total_len = swap16 (l + 8 + len);

	if ((len + 8) > (NET_PACKET_MTU + sizeof (proto_udp_t) + 1)) {
		Printf("UDP -> data lenght is exceed: %d/%d bytes", len+8, NET_PACKET_MTU + sizeof (proto_udp_t) + 1);
		return 0;
	}
	
	char *buf_udp = (char *) &buf_udp_prealloc;

	if (!buf_udp)
		return 0;

	memcpy (buf_udp, (char *) udp, 8);
	memcpy (buf_udp+8, data, len);

	buf_udp[8 + len] = '\0';

	/* calculate checksum and put it to udp header */
	// FIXME: wrong checksum
	proto_udp_t *udp_ = (proto_udp_t *) buf_udp;
	udp_->checksum = checksum16_udp (conn->ip_source, conn->ip_dest, buf_udp, 8 + len); // checksum16 (buf_udp, 8 + len);

	/* send udp header+data to ip layer */
	unsigned ret = net_proto_ip_send (conn->netif, packet, ip, (char *) buf_udp, 8 + len);
	return ret;
}


/* Create new UDP connection */
int net_proto_udp_conn_add (fd_t *fd)
{
	proto_udp_conn_t *conn;

	/* alloc and init context */
	conn = (proto_udp_conn_t *) NEW (sizeof (proto_udp_conn_t));

	if (!conn) {
		errno_set (ENOMEM);
		return -1;
	}

	memset (conn, 0, sizeof (proto_udp_conn_t));

	conn->flags = 0;

	conn->fd = fd->id;

	/* add into list */
	conn->next = &proto_udp_conn_list;
	conn->prev = proto_udp_conn_list.prev;
	conn->prev->next = conn;
	conn->next->prev = conn;

	return 0;
}

/* Setup new connection */
int net_proto_udp_conn_set (proto_udp_conn_t *conn, netif_t *eth, net_port port_source, net_ipv4 ip_dest, net_port port_dest)
{
	if (!conn || !eth)
		return 0;

	conn->ip_source = eth->ip;
	conn->ip_dest = ip_dest;

	conn->port_source = port_source;
	conn->port_dest = port_dest;

	conn->netif = eth;

	conn->bind = 0;
	conn->state = 0;

	conn->len = 0;
	conn->data = 0;

	return 1;
}

/* Delete existing connection from list */
unsigned net_proto_udp_conn_del (proto_udp_conn_t *conn)
{
	if (!conn)
		return 0;

	if (conn->len)
		DEL (conn->data);

	conn->len = 0;

	conn->next->prev = conn->prev;
	conn->prev->next = conn->next;

	DEL (conn);

	return 1;
}

proto_udp_conn_t *net_proto_udp_conn_check (net_ipv4 ip_source, net_port port_source, net_ipv4 ip_dest, net_port port_dest, unsigned char *ret)
{
	*ret = 0;
	proto_udp_conn_t *conn = NULL;
	proto_udp_conn_t *conn_ret = NULL;
	
	for (conn = proto_udp_conn_list.next; conn != &proto_udp_conn_list; conn = conn->next)
	{
		if(conn->ip_dest == INADDR_BROADCAST)
		{
			if( conn->port_source == port_source)
			{
				*ret = 2;
				return conn;
			}
		}
	}

	if (ip_source != INADDR_BROADCAST) {
		for (conn = proto_udp_conn_list.next; conn != &proto_udp_conn_list; conn = conn->next) {
			if (conn->ip_source == ip_source && conn->port_source == port_source) {
				if (conn->ip_dest == ip_dest) {
					*ret = 2;

					/*if (conn->bind) {
						*ret = 1;Printf ("OO11122\n");
						conn->state = PROTO_UDP_CONN_STATE_NEW;
					}*/

					return conn;
				}

				*ret = 1;

				conn_ret = conn;
			}
		}
	} else {	/* broadcast packet */
		for (conn = proto_udp_conn_list.next; conn != &proto_udp_conn_list; conn = conn->next) {
			if (conn->port_source == port_source && conn->port_dest == port_dest) {
				conn->ip_source = ip_dest;
				conn->ip_dest = ip_source;

				*ret = 2;
				return conn;
			}
		}
	}

	if (*ret == 1) {
		//if (!conn_ret->bind) {
			conn_ret = 0;

			for (conn = proto_udp_conn_list.next; conn != &proto_udp_conn_list; conn = conn->next) {
				if (conn->bind) {
					if (conn->ip_source == ip_source && conn->port_source == port_source) {
						conn_ret = conn;
						conn->state = PROTO_UDP_CONN_STATE_NEW;
					}
				}
			}
		}

	return conn_ret;
}

proto_udp_conn_t *net_proto_udp_conn_find (int fd)
{
	proto_udp_conn_t *conn = NULL;
	for (conn = proto_udp_conn_list.next; conn != &proto_udp_conn_list; conn = conn->next) {
		if (conn->fd == fd)
			return conn;
	}
	
	return 0;
}

/* init of udp protocol */
unsigned init_net_proto_udp ()
{
	proto_udp_conn_list.next = &proto_udp_conn_list;
	proto_udp_conn_list.prev = &proto_udp_conn_list;

	return 1;
}
