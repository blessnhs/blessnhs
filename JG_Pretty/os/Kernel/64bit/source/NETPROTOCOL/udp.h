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

#ifndef _UDP_H
#define _UDP_H

#include "ip.h"

#define PROTO_UDP_CONN_STATE_NEW	0x1

/* UDP connection structure */
typedef struct proto_udp_conn_context {
	struct proto_udp_conn_context *next, *prev;

	net_ipv4 ip_source;
	net_ipv4 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned short flags;

	unsigned char bind;

	unsigned char state;
	
	unsigned short fd;

	netif_t *netif;

	unsigned len;
	char *data;
} proto_udp_conn_t;

/* UDP connection structure */
typedef struct proto_udp6_conn_context {
	struct proto_udp6_conn_context *next, *prev;

	net_ipv6 ip_source;
	net_ipv6 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned short flags;

	unsigned char bind;

	unsigned short fd;

	netif_t *netif;

	unsigned len;
	char *data;
} proto_udp6_conn_t;

/* UDP layer structure */
typedef struct proto_udp_t {
	net_port port_source;		// 2
	net_port port_dest;		// 4

	unsigned short len;		// 6

	unsigned short checksum;	// 8
} proto_udp_t;


/* externs */
extern int net_proto_udp_anycast (int fd);
extern int net_proto_udp_port (int fd, net_port port);
extern unsigned init_net_proto_udp ();
extern unsigned init_net_proto_udp6 ();

#endif
