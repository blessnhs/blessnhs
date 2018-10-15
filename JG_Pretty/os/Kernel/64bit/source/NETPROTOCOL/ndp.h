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

#ifndef _NDP_H
#define _NDP_H

#include "ip.h"

#define NET_NDP_TYPE_RSOL		0x85
#define NET_NDP_TYPE_RADVERT		0x86
#define NET_NDP_TYPE_NBSOL		0x87
#define NET_NDP_TYPE_NBADVERT		0x88

typedef struct ndp_cache_context {
	struct ndp_cache_context *next, *prev;

	net_ipv6 ip;
	mac_addr_t mac;
} ndp_cache_t;

/* ICMP - NDP layer structure */
typedef struct proto_icmp_ndp2_t {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned char hop_limit;
	unsigned char flags;
	unsigned short rlifetime;
	unsigned reach_time;
	unsigned retr_time;
	
	/* 1. ICMPv6 options */
	unsigned char ptype;
	unsigned char length;
	unsigned char prefix_len;
	unsigned char pflags;
	unsigned valid_ltime;
	unsigned pref_ltime;
	unsigned unused;
	net_ipv6 prefix;
	
	/* 2. ICMPv6 options */
	unsigned char ll_type;
	unsigned char ll_length;
	mac_addr_t ll_mac;
} proto_icmp_ndp2_t;

typedef struct proto_ndp_t {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned flags;
	net_ipv6 target;
	
	/* ICMPv6 options */
	unsigned char ll_type;
	unsigned char ll_length;
	mac_addr_t ll_mac;
} proto_ndp_t;

extern unsigned ndp_cache_add (mac_addr_t mac, net_ipv6 ip);
extern unsigned ndp_cache_get (net_ipv6 ip, mac_addr_t *mac);
extern unsigned init_proto_ndp ();

#endif
