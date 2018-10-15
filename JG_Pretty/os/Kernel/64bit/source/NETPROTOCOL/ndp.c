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


#include "eth.h"
#include "net.h"
#include "if.h"
#include "packet.h"
#include "ip.h"
#include "icmp.h"
#include "ndp.h"
#include "tun.h"
#include "checksum.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

ndp_cache_t ndp_cache_list;
mac_addr_t ndp_macgw;

extern unsigned long timer_ticks;

/* prototype */


unsigned ndp_send_nbrequest (netif_t *netif, net_ipv6 dest)
{
	/* packet header */
	packet_t *packet = (packet_t *) NEW (sizeof (packet_t));

	if (!packet)
		return 0;

	char id[2];
	memcpy (&id, (void *) &dest[7], sizeof (unsigned short));

	mac_addr_t mac_dest;
	mac_dest[0] = 0x33;
	mac_dest[1] = 0x33;
	mac_dest[2] = 0xff;
	mac_dest[3] = 0x0;
	mac_dest[4] = 0x0;
	mac_dest[5] = id[1];

	memcpy (&packet->mac_source, netif->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	/* ip layer */
	proto_ipv6_t *ip = (proto_ipv6_t *) NEW (sizeof (proto_ipv6_t));

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (32);
	ip->nhead = NET_PROTO_IP_TYPE_ICMP6;
	ip->hop = 0xff;

	memcpy (ip->ip_source, (void *) netif->ipv6, sizeof (net_ipv6));

//	if (dest[0] == swap16 (0xff02))

	NET_IPV6_TO_ADDR (ip->ip_dest, 0xff02, 0x0, 0x0, 0x0, 0x0, 0x0001, 0xff00, swap16 (dest[7]));
	//memcpy (ip->ip_dest, (void *) dest, sizeof (net_ipv6));

	/* icmp layer */
	proto_ndp_t *ndp = (proto_ndp_t *) NEW (sizeof (proto_ndp_t));

	if (!ndp)
		return 0;

	memset ((char *) ndp, 0, sizeof (proto_ndp_t));

	ndp->type = NET_NDP_TYPE_NBSOL;
	ndp->code = 0;
	memcpy (ndp->target, (void *) dest, sizeof (net_ipv6));

	ndp->ll_type = 1;
	ndp->ll_length = 1;
	memcpy (&ndp->ll_mac, netif->dev->dev_addr, 6);

	ndp->checksum = checksum16_ipv6 (ip->ip_source, ip->ip_dest, ndp, sizeof (proto_ndp_t), NET_PROTO_IP_TYPE_ICMP6);

	unsigned ret = net_proto_ipv6_send (netif, packet, ip, (char *) ndp, sizeof (proto_ndp_t));

	DEL (ndp);
	DEL (ip);
	DEL (packet);

	return 1;
}


int ndp_send_nbreply (netif_t *i, mac_addr_t target_mac, net_ipv6 target_ipaddr)
{
	/* packet header */
	packet_t *packet = (packet_t *) NEW (sizeof (packet_t));

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, i->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, target_mac, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	/* ip layer */
	proto_ipv6_t *ip = (proto_ipv6_t *) NEW (sizeof (proto_ipv6_t));

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (32);
	ip->nhead = NET_PROTO_IP_TYPE_ICMP6;
	ip->hop = 0xff;

	memcpy (ip->ip_source, (void *) i->ipv6, sizeof (net_ipv6));

//	if (dest[0] == swap16 (0xff02))

	memcpy (ip->ip_dest, (void *) target_ipaddr, sizeof (net_ipv6));

	/* icmp layer */
	proto_ndp_t *ndp = (proto_ndp_t *) NEW (sizeof (proto_ndp_t));

	if (!ndp)
		return 0;

	memset ((char *) ndp, 0, sizeof (proto_ndp_t));

	ndp->type = NET_NDP_TYPE_NBADVERT;
	ndp->code = 0;
	ndp->flags = swap32 (0x60000000);
	memcpy (ndp->target, (void *) i->ipv6, sizeof (net_ipv6));

	ndp->ll_type = 2;
	ndp->ll_length = 1;
	memcpy (&ndp->ll_mac, i->dev->dev_addr, 6);

	ndp->checksum = checksum16_ipv6 (ip->ip_source, ip->ip_dest, ndp, sizeof (proto_ndp_t), NET_PROTO_IP_TYPE_ICMP6);

	unsigned ret = net_proto_ipv6_send (i, packet, ip, (char *) ndp, sizeof (proto_ndp_t));

	DEL (ndp);
	DEL (ip);
	DEL (packet);

	return 1;
}

int ndp_send_rreply (netif_t *i, proto_icmp_ndp2_t *ndp, net_ipv6 source, net_ipv6 dest)
{
	/*Printf ("IP adresa je: ");
	net_proto_ipv6_print (source);
	Printf ("\n");*/

	net_ipv6 bcast_ip;

	NET_IPV6_TO_ADDR (bcast_ip, 0xfe80, 0, 0, 0, 0, 0, 0, 0);

	if (!net_proto_ipv6_cmp_prefix (source, bcast_ip, 64)) {
		return 0;
	}

	/* local IPv6 address - we can change it to public IPv6 */
	if (!net_proto_ipv6_network (i->ipv6)) {
		if (ndp->prefix_len != 64)
			return 0;

		net_ipv6 ipv6;

/*
eth3      Zapouzdření:Ethernet  HWadr 00:E0:4D:7A:D3:48  
          inet6-adr: 2001:15c0:661d:0:2e0:4dff:fe7a:d348/64 Rozsah:Globál
          inet6-adr: fe80::2e0:4dff:fe7a:d348/64 Rozsah:Linka
*/

		unsigned l = ndp->prefix_len/8;

		memcpy (&ipv6, (void *) ndp->prefix, l);

		//memcpy ((char *) &ipv6+ndp->prefix_len/8, ndp->prefix, ndp->prefix_len);
		/*unsigned i;
		for (i = l/8; i < 8; i ++) {
			ipv6[i] = 
		}*/
		ipv6[4] = 0x2 | i->dev->dev_addr[1] << 8;
		ipv6[5] = 0xff << 8 | i->dev->dev_addr[2];
		ipv6[6] = 0xfe | i->dev->dev_addr[3] << 8;
		ipv6[7] = i->dev->dev_addr[4] | i->dev->dev_addr[5] << 8;

		netif_ipv6_addr (i, ipv6, IF_CFG_TYPE_RADVERT);
		
		//NET_IPV6_TO_ADDR (ipv6, 0xfc00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1);


		netif_gwv6_addr (i, ndp->prefix);
		memcpy (&ndp_macgw, &ndp->ll_mac, sizeof (mac_addr_t));
	}

	return 1;
#ifdef TEST
	/* packet header */
	packet_t *packet = (packet_t *) NEW (sizeof (packet_t));

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, i->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, target_mac, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	/* ip layer */
	proto_ipv6_t *ip = (proto_ipv6_t *) NEW (sizeof (proto_ipv6_t));

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (32);
	ip->nhead = NET_PROTO_IP_TYPE_ICMP6;
	ip->hop = 0xff;

	memcpy (ip->ip_source, (void *) i->ipv6, sizeof (net_ipv6));

//	if (dest[0] == swap16 (0xff02))

	memcpy (ip->ip_dest, (void *) source, sizeof (net_ipv6));

	/* icmp layer */
	proto_ndp_adv_t *ndp = (proto_ndp_adv_t *) NEW (sizeof (proto_ndp_adv_t));

	if (!ndp)
		return 0;

	memset ((char *) ndp, 0, sizeof (proto_ndp_sol_t));

	ndp->type = NET_NDP_TYPE_RSOL;
	ndp->code = 0;
	ndp->flags = swap32 (0x60000000);
	memcpy (ndp->target, (void *) i->ipv6, sizeof (net_ipv6));

	ndp->ll_type = 2;
	ndp->ll_length = 1;
	memcpy (&ndp->ll_mac, i->dev->dev_addr, 6);

	ndp->checksum = checksum16_ipv6 (ip->ip_source, ip->ip_dest, ndp, sizeof (proto_ndp_adv_t), NET_PROTO_IP_TYPE_ICMP6);

	unsigned ret = net_proto_ipv6_send (i, packet, ip, (char *) ndp, sizeof (proto_ndp_sol_t));

	DEL (ndp);
	DEL (ip);
	DEL (packet);
#endif
	return 1;
}

/* NDP Cache */
unsigned ndp_cache_add (mac_addr_t mac, net_ipv6 ip)
{
	ndp_cache_t *cache;
	for (cache = ndp_cache_list.next; cache != &ndp_cache_list; cache = cache->next) {
		if (net_proto_ipv6_cmp (cache->ip, ip)) {
			//Printf ("PRDPRD\n");
			return 0;
		}
	}

	//Printf ("hmm: pridavam novou mac %x:%x:%x:%x:%x:%x a ip: ", (unsigned char) mac[0], (unsigned char) mac[1], (unsigned char) mac[2], (unsigned char) mac[3], (unsigned char) mac[4], (unsigned char) mac[5]);
	//net_proto_ipv6_print (ip);

	/* alloc and init context */
	cache = (ndp_cache_t *) NEW (sizeof (ndp_cache_t));

	if (!cache)
		return 0;

	memcpy (&cache->mac, mac, 6);

	memcpy (cache->ip, (void *) ip, sizeof (net_ipv6));

	/* add into list */
	cache->next = &ndp_cache_list;
	cache->prev = ndp_cache_list.prev;
	cache->prev->next = cache;
	cache->next->prev = cache;

	return 1;
}

unsigned ndp_cache_get (net_ipv6 ip, mac_addr_t *mac)
{
	ndp_cache_t *cache;

	if (net_proto_ipv6_network (ip)) {
		/*netif_t *eth = netif_findbyname ("eth0");

		if (!eth)
			return 0;

		for (cache = ndp_cache_list.next; cache != &ndp_cache_list; cache = cache->next) {
			if (net_proto_ipv6_cmp (cache->ip, eth->gwv6)) {
				memcpy (mac, cache->mac, 6);
				return 1;
			}
		}*/

		memcpy (mac, &ndp_macgw, 6);

		return 1;
	} else {
		for (cache = ndp_cache_list.next; cache != &ndp_cache_list; cache = cache->next) {
			if (net_proto_ipv6_cmp (cache->ip, ip)) {
				memcpy (mac, cache->mac, 6);
				return 1;
			}
		}
	}

	return 0;
}

unsigned init_proto_ndp ()
{
	ndp_cache_list.next = &ndp_cache_list;
	ndp_cache_list.prev = &ndp_cache_list;

	return 1;
}
