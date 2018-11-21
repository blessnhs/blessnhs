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

#ifndef _IP_H
#define _IP_H

#include "if.h"

typedef unsigned int net_ipv4;
typedef unsigned short net_ipv6[8];

#define NET_IPV4_TO_ADDR(a, b, c, d) \
	(((net_ipv4)(d) << 24) | (((net_ipv4)(c) & 0xff) << 16) | (((net_ipv4)(b) & 0xff) << 8) | ((net_ipv4)(a) & 0xff))

#define NET_PROTO_IP_TYPE_ICMP	0x1
#define NET_PROTO_IP_TYPE_ICMP6	0x3a
#define NET_PROTO_IP_TYPE_TCP	0x6
#define NET_PROTO_IP_TYPE_TCP6	0x6
#define NET_PROTO_IP_TYPE_UDP	0x11
#define NET_PROTO_IP_TYPE_UDP6	0x11
#define NET_PROTO_IP_TYPE_TUN6	0x29

#define NET_PROTO_IP_FLAG_TUNNEL 0x80


typedef struct proto_ip_t {
	unsigned char head_len:4;
	unsigned char ver:4;

	unsigned char res0;

	unsigned short total_len;

	unsigned short ident;

	unsigned char flags;

	unsigned char frag;
	
	unsigned char ttl;

	unsigned char proto;

	unsigned short checksum;

	net_ipv4 ip_source;
	net_ipv4 ip_dest;
} __attribute__((packed))  proto_ip_t;

typedef struct proto_ipv6_t {
	unsigned char ver;
	unsigned char tclass;
	unsigned short flabel;

	unsigned short pl_len;
  
	unsigned char nhead;

	unsigned char hop;

	net_ipv6 ip_source;
	net_ipv6 ip_dest;
} __attribute__((packed))  proto_ipv6_t;

extern void NET_IPV6_TO_ADDR (net_ipv6 ip, unsigned short a, unsigned short b, unsigned short c, unsigned short d, 
			  unsigned short e, unsigned short f, unsigned short g, unsigned short h);
extern unsigned net_proto_ipv6_cmp (net_ipv6 ip, net_ipv6 ip2);
extern void net_proto_ip_print (net_ipv4 ip);
extern net_ipv4 net_proto_ip_convert (char *ip);
extern unsigned net_proto_ip_network (net_ipv4 ip);
extern unsigned net_proto_ip_convert2 (net_ipv4 ip, char *ip_addr);
//extern unsigned net_proto_ip_esend (netif_t *netif, packet_t *packet, proto_ip_t *ip, char *buf, unsigned len);
//extern unsigned net_proto_ip_send (netif_t *netif, net_ipv4 ip_addr, char *buf, unsigned len);
extern unsigned init_net_packet ();
unsigned inet_addr (const char *src);

#endif
