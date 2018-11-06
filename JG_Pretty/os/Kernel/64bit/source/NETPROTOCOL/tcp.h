/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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

#ifndef _TCP_H
#define _TCP_H

#include "ip.h"

#define PROTO_TCP_CONN_STATE_ESTABILISH		0x1
#define PROTO_TCP_CONN_STATE_ESTABILISHED	0x2
#define PROTO_TCP_CONN_STATE_DISCONNECTED	0x4
#define PROTO_TCP_CONN_STATE_WAIT		0x8
#define PROTO_TCP_CONN_STATE_READY		0x10
#define PROTO_TCP_CONN_STATE_CLOSE		0x20
#define PROTO_TCP_CONN_STATE_ESTABILISHERROR	0x40

/* TCP connection structure */
typedef struct proto_tcp_conn_context {
	struct proto_tcp_conn_context *next, *prev;

	net_ipv4 ip_source;
	net_ipv4 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned seq;
	unsigned ack;

	unsigned short flags;

	unsigned char state;

	unsigned char bind;

	unsigned short fd;

	unsigned char cross;

	netif_t *netif;

	unsigned int offset;
	unsigned int len;
	char *data;
} proto_tcp_conn_t;

/* TCPv6 connection structure */
typedef struct proto_tcp6_conn_context {
	struct proto_tcp6_conn_context *next, *prev;

	net_ipv6 ip_source;
	net_ipv6 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned seq;
	unsigned ack;

	unsigned short flags;

	unsigned char state;

	unsigned char bind;

	unsigned short fd;

	unsigned char cross;

	netif_t *netif;

	unsigned short offset;
	unsigned short len;
	char *data;
} proto_tcp6_conn_t;

/* TCP server backlog structure */
typedef struct proto_tcp_backlog_context {
	struct proto_tcp_backlog_context *next, *prev;

	net_ipv4 ip;

	net_port port;

	unsigned seq;

	proto_tcp_conn_t *conn;
} proto_tcp_backlog_t;

/* TCPv6 server backlog structure */
typedef struct proto_tcp6_backlog_context {
	struct proto_tcp6_backlog_context *next, *prev;

	net_ipv6 ip;

	net_port port;

	unsigned seq;

	proto_tcp6_conn_t *conn;
} proto_tcp6_backlog_t;

/* TCP layer structure */
typedef struct proto_tcp_t {
	net_port port_source;		// 2
	net_port port_dest;		// 4

	unsigned seq;			// 8
	unsigned ack;			// 12

	unsigned char res:4;		// 13
	unsigned char data_offset:4;	

	unsigned char flags;		// 14

	unsigned short window;		// 16

	unsigned short checksum;	// 18

//	unsigned char *options;
} proto_tcp_t;


extern unsigned init_net_proto_tcp ();
extern unsigned init_net_proto_tcp6 ();

#endif
