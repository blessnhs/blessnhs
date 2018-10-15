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
#include "packet.h"
#include "ip.h"
#include "checksum.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

net_ipv4 tun6_ip;

#define DEFAULT_TUN6_ADDRESS	NET_IPV4_TO_ADDR (216,66,80,30)


unsigned tun6_addr (net_ipv4 tun6)
{
	tun6_ip = tun6;

	return 1;
}

unsigned tun6_addr_get ()
{
	return tun6_ip;
}

unsigned net_proto_tun6_send (netif_t *netif, packet_t *packet, proto_ipv6_t *ip, char *buf, unsigned len)
{
	char *buf_ip = (char *) NEW ((len + 1 + sizeof (proto_ipv6_t)));

	if (!buf_ip)
		return 0;

	unsigned l = sizeof (proto_ipv6_t);

	/* put ip header and buf data to one buffer */
	memcpy (buf_ip, (char *) ip, l);
	memcpy (buf_ip+l, buf, len);

	buf_ip[l+len] = '\0';


	packet->type = NET_PACKET_TYPE_IPV4;

	/* ip layer */
	proto_ip_t *ipv4 = (proto_ip_t *) NEW (sizeof (proto_ip_t));

	if (!ipv4)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ipv4->ver = 4;
	ipv4->head_len = 5;

	ipv4->flags = 0;
	ipv4->frag = 0;
	ipv4->ttl = 64;
	ipv4->checksum = 0;
	ipv4->proto = NET_PROTO_IP_TYPE_TUN6;
	ipv4->ip_source = netif->ip;
	ipv4->ip_dest = tun6_addr_get ();

	/* calculate total length of packet (ipv6/ip) */
	unsigned i = ipv4->head_len * 4;
	ipv4->total_len = swap16 (i+l+len);

	unsigned ret = net_proto_ip_send (netif, packet, ipv4, buf_ip, l+len);

	DEL (ipv4);
	DEL (buf_ip);

	return ret;
}

unsigned net_proto_tun6_handler (packet_t *packet, proto_ip_t *ip, char *buf, unsigned len)
{
	proto_ipv6_t *ipv6 = (proto_ipv6_t *) buf;

	/* ipv6 provide directly length of data - payload length */
	unsigned data_len = swap16 (ipv6->pl_len);

	//Printf ("tun6 -> 0x%x\n", ipv6->nhead);

	switch (ipv6->nhead) {
		case NET_PROTO_IP_TYPE_TCP6:
			return net_proto_tcp6_handler (packet, ipv6, buf+sizeof (proto_ipv6_t), data_len);
		case NET_PROTO_IP_TYPE_UDP6:
			return net_proto_udp6_handler (packet, ipv6, buf+sizeof (proto_ipv6_t), data_len);
		case NET_PROTO_IP_TYPE_ICMP6:
			return net_proto_icmp6_handler (packet, ipv6, buf+sizeof (proto_ipv6_t), data_len);
	}

	return 0;
}

unsigned init_net_proto_tun6 ()
{
	tun6_addr (0);

	return 1;
}
