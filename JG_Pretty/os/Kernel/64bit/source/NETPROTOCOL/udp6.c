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
#include "../console.h"
#include "../Utility.h"
#include "../DynamicMemory.h"
extern netif_t netif_list;

proto_udp6_conn_t proto_udp6_conn_list;

static proto_ipv6_t proto_ipv6_prealloc;
static proto_udp_t proto_udp_prealloc;
static packet_t packet_prealloc;
static char buf_udp_prealloc[NET_PACKET_MTU + sizeof (proto_udp_t) + 1];

/* prototypes */
proto_udp6_conn_t *net_proto_udp6_conn_check (net_ipv6 ip_source, net_port port_source, net_ipv6 ip_dest, net_port port_dest, unsigned char *ret);
int net_proto_udp6_read_cache (proto_udp6_conn_t *conn, char *data, unsigned len);
int net_proto_udp6_write (proto_udp6_conn_t *conn, char *data, unsigned len);
proto_udp6_conn_t *net_proto_udp6_conn_find (int fd);
int net_proto_udp6_conn_set (proto_udp6_conn_t *conn, netif_t *eth, net_port port_source, net_ipv6 ip_dest, net_port port_dest);
unsigned net_proto_udp6_conn_del (proto_udp6_conn_t *conn);
int net_proto_udp6_conn_add ();

/** UDPv6 protocol
 *  User-friendly socket functions
 */
int net_proto_udp6_socket (fd_t *fd)
{
	return net_proto_udp6_conn_add (fd);
}

int net_proto_udp6_connect (int fd, sockaddr_in6 *addr)
{
	int ret = -1;

	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		ret = net_proto_udp6_conn_set (conn, netif, swap16 (netif_port_get ()), addr->sin6_addr, addr->sin6_port);
		
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

int net_proto_udp6_send (int fd, char *msg, unsigned size)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_udp6_write (conn, msg, size);

	if (ret) {
		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			
		} else {
		/* non-blocking mode */
		/* TODO: ? */
			
		}
	}

	return size;
}

int net_proto_udp6_sendto (int fd, char *msg, unsigned size, sockaddr_in6 *to)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	unsigned len = size;
	
	unsigned mtu_udp = NET_PACKET_MTU - sizeof (proto_udp_t) - sizeof (net_ipv6) - 16;

check:
	if (size > mtu_udp) {
		int r = net_proto_udp6_sendto (fd, msg, mtu_udp, to);

		if (r <= 0)
			return r;
		
		msg += mtu_udp;
		size -= mtu_udp;
		
		goto check;
	}

	int ret = net_proto_udp6_write (conn, msg, size);

	if (ret) {
		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			
		} else {
		/* non-blocking mode */
		/* TODO: ? */
			
		}
	}

	return size;
}

int net_proto_udp6_recv (int fd, char *msg, unsigned size)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

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

int net_proto_udp6_recvfrom (int fd, char *msg, unsigned size, sockaddr_in6 *from)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -3;

	if (!msg)
		return -4;

	int ret = 0;

	//int ip = conn->ip_dest;
	
	memcpy (from->sin6_addr, (void *) conn->ip_dest, sizeof (net_ipv6));
	//kPrintf ("recvfrom -> ip: 0x%x\n", from->sin6_addr);

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

int net_proto_udp6_close (int fd)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_udp6_conn_del (conn);

	return ret;
}

int net_proto_udp6_fcntl (int fd, int cmd, long arg)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

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

int net_proto_udp6_bind (int fd, sockaddr_in6 *addr, socklen_t len)
{
	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		net_proto_udp6_conn_set (conn, netif, addr->sin6_port, 0, 0);
		conn->bind = 1;
		return 0;
	}

	return -1;
}

int net_proto_udp6_select (int readfd, int writefd, int exceptfd)
{
	int fd = -1;
  
	if (readfd)
		fd = readfd;
	else if (writefd)
		fd = writefd;
	else if (exceptfd)
		fd = exceptfd;

	proto_udp6_conn_t *conn = net_proto_udp6_conn_find (fd);

	if (!conn)
		return -1;

	/* Are some data available ? */
	if (conn->len)
		return 1;
	
	return 0;
}


/** UDP protocol
 *  hardcore code :P
 */

/* handler for received udp datagrams */
unsigned net_proto_udp6_handler (packet_t *packet, proto_ipv6_t *ip, char *buf, unsigned len)
{
	unsigned char ret;
	proto_udp_t *udp = (proto_udp_t *) buf;

	/* First check ip address and ports, that we want this data */
	proto_udp6_conn_t *conn = net_proto_udp6_conn_check (ip->ip_dest, udp->port_dest, ip->ip_source, udp->port_source, &ret);

	if (!conn && ret == 0)
		return 1;

	if (ret == 1)
		memcpy (conn->ip_dest, (void *) ip->ip_source, sizeof (net_ipv6));

	/* HACK: very ugly */
	conn->port_dest = udp->port_source;

	/* save data to buffer; +8 because udp packet is 8byte header and then data */
	int ret2 = net_proto_udp6_read_cache (conn, buf+8, len-8);

	if (ret2 < 1)
		return 0;

	return 1;
}

/* save received data to buffer */
int net_proto_udp6_read_cache (proto_udp6_conn_t *conn, char *data, unsigned len)
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

int net_proto_udp6_write (proto_udp6_conn_t *conn, char *data, unsigned len)
{
	if (!conn || !len || !data)
		return 0;

	mac_addr_t mac_dest;
	if (!tun6_addr_get ()) {
		unsigned get = ndp_cache_get (conn->ip_dest, &mac_dest);
	
		if (!get) {
			ndp_send_nbrequest (conn->netif, conn->ip_dest);
	
			unsigned i = 0;
			/* 100ms for waiting on ARP reply */
			while (i < 100) {
				get = ndp_cache_get (conn->ip_dest, &mac_dest);
	
				if (get)
					break;
	
				/* TODO: make better waiting for ARP reply */
				Sleep (1);
	
				Schedule ();
		
				i ++;
			}
	
			if (!get)
				return 0;
		}
	} else {
		/* IPv6 tunnel stuff */
		net_ipv4 tunnel = tun6_addr_get ();
		unsigned get = arp_cache_get (tunnel, &mac_dest);
	
		if (!get) {
			arp_send_request (conn->netif, tunnel);
	
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
	
			if (!get)
				return 0;
		}
	}

	/* packet header */
	packet_t *packet = (packet_t *) &packet_prealloc;

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, conn->netif->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	/* ip layer */
	proto_ipv6_t *ip = (proto_ipv6_t *) &proto_ipv6_prealloc;

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (len);
	ip->nhead = NET_PROTO_IP_TYPE_UDP6;
	ip->hop = 0xff;
	memcpy (ip->ip_source, (void *) conn->ip_source, sizeof (net_ipv6));
	memcpy (ip->ip_dest, (void *) conn->ip_dest, sizeof (net_ipv6));


	proto_udp_t *udp = (proto_udp_t *) &proto_udp_prealloc;

	if (!udp)
		return 0;

	udp->port_source = conn->port_source;
	udp->port_dest = conn->port_dest;
	udp->len = ip->pl_len;
	udp->checksum = 0;
	
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
	proto_udp_t *udp_ = (proto_udp_t *) buf_udp;
	udp_->checksum = checksum16_ipv6 (conn->ip_source, conn->ip_dest, buf_udp, 8 + len, NET_PROTO_IP_TYPE_UDP6);

	unsigned ret = 0;

	/* send udp header+data to ip layer */
	if (!tun6_addr_get ())
		ret = net_proto_ipv6_send (conn->netif, packet, ip, (char *) buf_udp, 8 + len);
	else
		ret = net_proto_tun6_send (conn->netif, packet, ip, (char *) buf_udp, 8 + len);

	return ret;
}


/* Create new UDP connection */
int net_proto_udp6_conn_add (fd_t *fd)
{
	proto_udp6_conn_t *conn;

	/* alloc and init context */
	conn = (proto_udp6_conn_t *) NEW (sizeof (proto_udp6_conn_t));

	if (!conn) {
		errno_set (ENOMEM);
		return -1;
	}

	memset (conn, 0, sizeof (proto_udp6_conn_t));

	conn->flags = 0;

	conn->fd = fd->id;

	/* add into list */
	conn->next = &proto_udp6_conn_list;
	conn->prev = proto_udp6_conn_list.prev;
	conn->prev->next = conn;
	conn->next->prev = conn;

	return 0;
}

/* Setup new connection */
int net_proto_udp6_conn_set (proto_udp6_conn_t *conn, netif_t *eth, net_port port_source, net_ipv6 ip_dest, net_port port_dest)
{
	if (!conn || !eth)
		return 0;

	memcpy (conn->ip_source, (void *) eth->ipv6, sizeof (net_ipv6));
	memcpy (conn->ip_dest, (void *) ip_dest, sizeof (net_ipv6));

	conn->port_source = port_source;
	conn->port_dest = port_dest;

	conn->netif = eth;

	conn->bind = 0;

	conn->len = 0;
	conn->data = 0;

	return 1;
}

/* Delete existing connection from list */
unsigned net_proto_udp6_conn_del (proto_udp6_conn_t *conn)
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

proto_udp6_conn_t *net_proto_udp6_conn_check (net_ipv6 ip_source, net_port port_source, net_ipv6 ip_dest, net_port port_dest, unsigned char *ret)
{
	*ret = 0;
	proto_udp6_conn_t *conn = NULL;
	proto_udp6_conn_t *conn_ret = NULL;

	for (conn = proto_udp6_conn_list.next; conn != &proto_udp6_conn_list; conn = conn->next) {
		if (conn->ip_source == ip_source && conn->port_source == port_source) {
			if (conn->ip_dest == ip_dest /*&& conn->port_dest == port_dest*/) {
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

			for (conn = proto_udp6_conn_list.next; conn != &proto_udp6_conn_list; conn = conn->next) {
				if (conn->bind) {
					if (conn->ip_source == ip_source && conn->port_source == port_source)
						conn_ret = conn;
				}
			}
		}

	return conn_ret;
}

proto_udp6_conn_t *net_proto_udp6_conn_find (int fd)
{
	proto_udp6_conn_t *conn = NULL;
	for (conn = proto_udp6_conn_list.next; conn != &proto_udp6_conn_list; conn = conn->next) {
		if (conn->fd == fd)
			return conn;
	}
	
	return 0;
}

/* init of udpv6 protocol */
unsigned init_net_proto_udp6 ()
{
	proto_udp6_conn_list.next = &proto_udp6_conn_list;
	proto_udp6_conn_list.prev = &proto_udp6_conn_list;

	return 1;
}
