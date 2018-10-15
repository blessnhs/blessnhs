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



#include "eth.h"
#include "net.h"
#include "if.h"
#include "ip.h"
#include "packet.h"
#include "socket.h"
#include "arp.h"
#include "../DynamicMemory.h"

arp_cache_t arp_cache_list;

unsigned net_proto_arp_handler (packet_t *packet, char *buf, unsigned len)
{
//	Printf("net_proto_arp_handler receive \n");

	proto_arp_t *arp = (proto_arp_t *) buf;

	if (arp->hard_type != ARP_HARD_TYPE_ETHERNET)
		return 0;

	if (arp->prot_type != NET_PACKET_TYPE_IPV4)
		return 0;

	/* ARP request */
	if (arp->op == ARP_OP_REQUEST) {
		//Printf ("ARP - REQUEST: %02x:%02x:%02x:%02x:%02x:%02x\n", (unsigned char) arp->sender_ethernet[0], (unsigned char) arp->sender_ethernet[1], (unsigned char) arp->sender_ethernet[2], (unsigned char) arp->sender_ethernet[3], (unsigned char) arp->sender_ethernet[4], (unsigned char) arp->sender_ethernet[5]);
		/* TODO: melo by to projit vsechny rozhrani */
		netif_t *netif = netif_findbyname ("eth0");

		/* send ARP reply */
		if (arp->target_ipv4 == netif->ip)
			arp_send_reply (netif, arp->sender_ethernet, arp->sender_ipv4);

		return 1;
	}

	if (arp->op == ARP_OP_REPLY) {
		//Printf ("ARP - REPLY: %02x:%02x:%02x:%02x:%02x:%02x\n", (unsigned char) arp->sender_ethernet[0], (unsigned char) arp->sender_ethernet[1], (unsigned char) arp->sender_ethernet[2], (unsigned char) arp->sender_ethernet[3], (unsigned char) arp->sender_ethernet[4], (unsigned char) arp->sender_ethernet[5]);

		arp_cache_add (arp->sender_ethernet, arp->sender_ipv4);

		return 1;
	}

	return 0;
}

int arp_send_request (netif_t *i, net_ipv4 target_ipaddr)
{	
	proto_arp_t *arp = (proto_arp_t *) NEW (sizeof (proto_arp_t));
	
	if (!arp)
	{
		Printf("arp_send_request !arp \n");
		return 0;
	}

	arp->hard_type = ARP_HARD_TYPE_ETHERNET;
	arp->prot_type = NET_PACKET_TYPE_IPV4;
	arp->hard_size = 6;
	arp->prot_size = 4;
	arp->op = ARP_OP_REQUEST;
	memcpy (&arp->sender_ethernet, i->dev->dev_addr, 6);
	arp->sender_ipv4 = i->ip;
	memset (&arp->target_ethernet, 0, 6);

	/* internet connection */
	if (net_proto_ip_network (target_ipaddr))
		arp->target_ipv4 = i->gw;
	else
	/* intranet connection */
		arp->target_ipv4 = target_ipaddr;

	packet_t *packet = (packet_t *) NEW (sizeof (packet_t));

	if (!packet)
		return 0;

	memset (&packet->mac_dest, 0xff, 6);
	memcpy (&packet->mac_source, i->dev->dev_addr, 6);

	packet->type = NET_PACKET_TYPE_ARP;

	int ret = net_packet_send (i, packet, (char *) arp, 28);

	DEL (packet);
	DEL (arp);

	return 1;
}

int arp_send_reply (netif_t *i, mac_addr_t target_mac, net_ipv4 target_ipaddr)
{
	proto_arp_t *arp = (proto_arp_t *) NEW (sizeof (proto_arp_t));

	if (!arp)
		return 0;

	arp->hard_type = ARP_HARD_TYPE_ETHERNET;
	arp->prot_type = NET_PACKET_TYPE_IPV4;
	arp->hard_size = 6;
	arp->prot_size = 4;
	arp->op = ARP_OP_REPLY;
	memcpy (&arp->sender_ethernet, i->dev->dev_addr, 6);
	arp->sender_ipv4 = i->ip;
	memcpy (&arp->target_ethernet, target_mac, 6);
	arp->target_ipv4 = target_ipaddr;

	packet_t *packet = (packet_t *) NEW (sizeof (packet_t));

	if (!packet)
		return 0;

	memcpy (&packet->mac_dest, target_mac, 6);
	memcpy (&packet->mac_source, i->dev->dev_addr, 6);

	packet->type = NET_PACKET_TYPE_ARP;

	int ret = net_packet_send (i, packet, (char *) arp, 28);

	DEL (packet);
	DEL (arp);

	return 1;
}

/* ARP Cache */
net_ipv4 arp_cache_add (mac_addr_t mac, net_ipv4 ip)
{
	arp_cache_t *cache;
	for (cache = arp_cache_list.next; cache != &arp_cache_list; cache = cache->next) {
		if (cache->ip == ip)
			return 0;
	}

	/* alloc and init context */
	cache = (arp_cache_t *) NEW (sizeof (arp_cache_t));

	if (!cache)
		return 0;

	memcpy (&cache->mac, mac, 6);

	cache->ip = ip;

	/* add into list */
	cache->next = &arp_cache_list;
	cache->prev = arp_cache_list.prev;
	cache->prev->next = cache;
	cache->next->prev = cache;

	return 1;
}

net_ipv4 arp_cache_get (net_ipv4 ip, mac_addr_t *mac)
{
	arp_cache_t *cache;

	if (net_proto_ip_network (ip)) {
		netif_t *eth = netif_findbyname ("eth0");

		if (!eth)
			return 0;

		for (cache = arp_cache_list.next; cache != &arp_cache_list; cache = cache->next) {
			if (cache->ip == eth->gw) {
				memcpy (mac, cache->mac, 6);
				return 1;
			}
		}
	} else {
		for (cache = arp_cache_list.next; cache != &arp_cache_list; cache = cache->next) {
			if (cache->ip == ip) {
				memcpy (mac, cache->mac, 6);
				return 1;
			}
		}
	}

	return 0;
}

net_ipv4 init_proto_arp ()
{
	arp_cache_list.next = &arp_cache_list;
	arp_cache_list.prev = &arp_cache_list;
	
	mac_addr_t mac_addr;
	memset (mac_addr, 0xff, 6);
	arp_cache_add (mac_addr, INADDR_BROADCAST);
	arp_cache_add (mac_addr, INADDR_ANY);
	
	return 1;
}
