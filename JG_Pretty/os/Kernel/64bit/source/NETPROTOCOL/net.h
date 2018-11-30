/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2009  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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

#ifndef _NET_H
#define _NET_H

#include "eth.h"

#define SOCK_FLAG_TIMEOUT 1

#define NETADDR_TO_IPV4(naddr) (*(ipv4_addr_t *)(&((&(naddr))->addr[0])))
#define IPV4_DOTADDR_TO_ADDR(a, b, c, d) \
	(((ipv4_addr_t)(a) << 24) | (((ipv4_addr_t)(b) & 0xff) << 16) | (((ipv4_addr_t)(c) & 0xff) << 8) | ((ipv4_addr_t)(d) & 0xff))

enum {
	SOCK_PROTO_NULL = 0,
	SOCK_PROTO_UDP,
	SOCK_PROTO_TCP
};

enum {
	ADDR_TYPE_NULL = 0,
	ADDR_TYPE_ETHERNET,
	ADDR_TYPE_IP
};

enum {
	IP_PROT_ICMP = 1,
	IP_PROT_TCP = 6,
	IP_PROT_UDP = 17,
};

typedef unsigned ipv4_addr_t;
typedef unsigned short net_port;
/*
#define ntohs(n) \
	((((unsigned short)(n) & 0xff) << 8) | ((unsigned short)(n) >> 8))
#define htons(h) \
	((((unsigned short)(h) & 0xff) << 8) | ((unsigned short)(h) >> 8))

#define ntohl(n) \
	(((unsigned)(n) << 24) | (((unsigned)(n) & 0xff00) << 8) |(((unsigned)(n) & 0x00ff0000) >> 8) | ((unsigned)(n) >> 24))
#define htonl(h) \
	(((unsigned)(h) << 24) | (((unsigned)(h) & 0xff00) << 8) |(((unsigned)(h) & 0x00ff0000) >> 8) | ((unsigned)(h) >> 24))
*/
#endif
