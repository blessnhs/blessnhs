/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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
#include "icmp.h"
#include "checksum.h"
#include "../Utility.h"

static unsigned net_icmp_ping = 0;

static proto_ip_t proto_ip_prealloc;
static char proto_icmp_prealloc[sizeof (proto_icmp_t)+57];
static packet_t packet_prealloc;
static char buf_icmp_prealloc[NET_PACKET_MTU + sizeof (proto_icmp_t) + 1];

/* prototype */
unsigned net_proto_icmp_reply (netif_t *netif, packet_t *packet, proto_ip_t *ip, proto_icmp_t *icmp, char *buf, unsigned len);
unsigned net_proto_icmp_request (netif_t *netif, net_ipv4 dest);

unsigned net_proto_icmp_ping (netif_t *netif, net_ipv4 ip)
{
	if (!netif)
		return 2;

	unsigned ret = net_proto_icmp_request (netif, ip);

	if (!ret)
		return 0;

	unsigned long time = GetTickCount();

	while (!net_icmp_ping) {
		/* timeout 1s - 1000ms */
		if ((GetTickCount()-time) > 1000)
			return 0;

		Schedule ();
	}

	net_icmp_ping = 0;

	return 1;
}

unsigned net_proto_icmp_handler (packet_t *packet, proto_ip_t *ip, char *buf, unsigned len)
{
	proto_icmp_t *icmp = (proto_icmp_t *) buf;

	if (icmp->type == NET_ICMP_TYPE_PING_REQUEST) {
		//kPrintf ("ICMP->Request !\n");
	
		/* TODO: melo by to projit vsechny rozhrani */
		netif_t *netif = netif_findbyname ("eth0");
	
		if (ip->ip_dest == netif->ip) {
			//Printf ("ICMP->Request from ");
			//net_proto_ip_print (ip->ip_source);
			//Printf (" !\n");
	
			return net_proto_icmp_reply (netif, packet, ip, icmp, buf+8, len-12);
		}

		return 0;
	}

	if (icmp->type == NET_ICMP_TYPE_PING_REPLY) {
		/* TODO: make it better :) */
		net_icmp_ping = 1;
		return 1;
	}

	return 0;
}

unsigned net_proto_icmp_request (netif_t *netif, net_ipv4 dest)
{
	//Printf ("ICMP request: (%d) %s\n", len, buf);

	mac_addr_t mac_dest;
	unsigned get = arp_cache_get (dest, &mac_dest);

	if (!get) {
		arp_send_request (netif, dest);

		unsigned i = 0;
		/* 100ms for waiting on ARP reply */
		while (i < 100) {
			get = arp_cache_get (dest, &mac_dest);

			if (get)
				break;

			/* TODO: make better waiting for ARP reply */
			Sleep(1000);

			Schedule ();
	
			i ++;
		}

		if (!get)
			return 0;
	}

	/* packet header */
	packet_t *packet = (packet_t *) &packet_prealloc;

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, netif->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV4;

	/* ip layer */
	proto_ip_t *ip = (proto_ip_t *) &proto_ip_prealloc;

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 4;
	ip->head_len = 5;
	ip->total_len = swap16 (84);
	ip->flags = 0;
	ip->frag = 0;
	ip->ttl = 64;
	ip->checksum = 0;
	ip->proto = NET_PROTO_IP_TYPE_ICMP;
	ip->ip_source = netif->ip;
	ip->ip_dest = dest;

	/* icmp layer */
	proto_icmp_t *icmp = (proto_icmp_t *) &proto_icmp_prealloc;

	if (!icmp)
		return 0;

	memset ((char *) icmp+8, 0, 56);

	icmp->type = NET_ICMP_TYPE_PING_REQUEST;
	icmp->code = 0;
	icmp->seq = 0;
	icmp->checksum = 0;

	icmp->checksum = checksum16 (icmp, sizeof (proto_icmp_t)+56);

	return net_proto_ip_send (netif, packet, ip, (char *) icmp, sizeof (proto_icmp_t)+56);
}

unsigned net_proto_icmp_reply (netif_t *netif, packet_t *packet, proto_ip_t *ip, proto_icmp_t *icmp, char *buf, unsigned len)
{
	//Printf ("ICMP reply: (%d) %s\n", len, buf);

	mac_addr_t mac_dest;
	mac_addr_t mac_source;

	/* cross mac and ip address, because this packet have to go back */
	memcpy (&mac_dest, packet->mac_source, 6);
	memcpy (&mac_source, packet->mac_dest, 6);

	net_ipv4 ip_dest = ip->ip_source;
	net_ipv4 ip_source = ip->ip_dest;

	/* assign new values to old structures */
	memcpy (&packet->mac_source, mac_source, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);

	/* TODO: identification, flags, etc */

	ip->ip_dest = ip_dest;
	ip->ip_source = ip_source;
	ip->checksum = 0;

	icmp->type = NET_ICMP_TYPE_PING_REPLY;
	icmp->checksum = 0;

	char *buf_icmp = (char *) &buf_icmp_prealloc;

	if (!buf_icmp)
		return 0;

	unsigned l = sizeof (proto_icmp_t);

	memcpy (buf_icmp, (char *) icmp, l);
	memcpy (buf_icmp+l, buf, len);

	buf_icmp[l+len] = '\0';

	/* calculate checksum and put it to icmp header */
	proto_icmp_t *icmp_ = (proto_icmp_t *) buf_icmp;
	icmp_->checksum = checksum16 (buf_icmp, len+l);

	return net_proto_ip_send (netif, packet, ip, buf_icmp, len+l);
}

