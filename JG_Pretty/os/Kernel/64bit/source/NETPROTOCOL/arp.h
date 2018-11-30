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

#ifndef _ARP_H
#define _ARP_H

#include "net.h"
#include "ip.h"

#define MIN_ARP_SIZE 28

#define	__PACKED__ __attribute__ ((__packed__))


typedef struct arp_cache_context {
	struct arp_cache_context *next, *prev;

	net_ipv4 ip;
	mac_addr_t mac;
} arp_cache_t;

typedef struct proto_arp_t {
	unsigned short hard_type;
	unsigned short prot_type;
	unsigned char hard_size;
	unsigned char prot_size;
	unsigned short op;	
	mac_addr_t sender_ethernet;
	net_ipv4 sender_ipv4;
	mac_addr_t target_ethernet;
	net_ipv4 target_ipv4;
} __PACKED__ proto_arp_t;

enum {
	ARP_OP_REQUEST = 0x100,
	ARP_OP_REPLY = 0x200,
	ARP_OP_RARP_REQUEST,
	ARP_OP_RARP_REPLY
};

enum {
	ARP_HARD_TYPE_ETHERNET = 0x100
};

net_ipv4 arp_cache_add (mac_addr_t mac, net_ipv4 ip);
net_ipv4 arp_cache_get (net_ipv4 ip, mac_addr_t *mac);
net_ipv4 init_proto_arp ();

#endif
