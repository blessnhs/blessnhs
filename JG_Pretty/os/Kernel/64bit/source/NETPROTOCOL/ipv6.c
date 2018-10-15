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
#include "../Utility.h"
#include "../DynamicMemory.h"

void NET_IPV6_TO_ADDR (net_ipv6 ip, unsigned short a, unsigned short b, unsigned short c, unsigned short d, 
			  unsigned short e, unsigned short f, unsigned short g, unsigned short h) {

	unsigned short buf[8];

	buf[0] = swap16 (a);
	buf[1] = swap16 (b);
	buf[2] = swap16 (c);
	buf[3] = swap16 (d);
	buf[4] = swap16 (e);
	buf[5] = swap16 (f);
	buf[6] = swap16 (g);
	buf[7] = swap16 (h);

	memcpy (ip, (void *) buf, sizeof (net_ipv6));
}

void net_proto_ipv6_print (net_ipv6 ip)
{
	unsigned i = 0;

	for (i = 0; i < 7; i ++) {
		if (ip[i])
			Printf ("%04x:", swap16 (ip[i]));
		else
			Printf ("0:");
	}

	if (ip[7])
		Printf ("%x", swap16 (ip[7]));
	else
		Printf ("0");
}

unsigned net_proto_ipv6_cmp (net_ipv6 ip, net_ipv6 ip2)
{
	unsigned i;
	for (i = 0; i < 8; i ++)
		if (ip[i] != ip2[i])
			return 0;

	return 1;
}

unsigned net_proto_ipv6_cmp_prefix (net_ipv6 ip, net_ipv6 ip2, unsigned char prefix)
{
	unsigned i;
	for (i = 0; i < 8; i ++) {
		if (i >= prefix/16)
			break;

		if (ip[i] != ip2[i])
			return 0;
	}

	return 1;
}

unsigned net_proto_ipv6_convert (net_ipv6 ipv6, char *ip)
{
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short d;
	unsigned short e;
	unsigned short f;
	unsigned short g;
	unsigned short h;

	unsigned j = 0;
	unsigned i = 0;
	unsigned y = strlen (ip);

	if (!y)
		return 0;

	unsigned k[8];

	while (i < y) {
		if (ip[i] == ':') {
			ip[i] = '\0';
			k[j] = i+1;
			j ++;
		}

		i ++;
	}

	if (j != 7)
		return 0;

	char *endptr;

	a = strtol (ip, &endptr, 16);
	b = strtol (ip+k[0], &endptr, 16);
	c = strtol (ip+k[1], &endptr, 16);
	d = strtol (ip+k[2], &endptr, 16);
	e = strtol (ip+k[3], &endptr, 16);
	f = strtol (ip+k[4], &endptr, 16);
	g = strtol (ip+k[5], &endptr, 16);
	h = strtol (ip+k[6], &endptr, 16);

	NET_IPV6_TO_ADDR (ipv6, a, b, c, d, e, f, g, h);

	return 1;
}

/*unsigned net_proto_ip_convert2 (net_ipv4 ip, char *ip_addr)
{
	if (!ip_addr)
		return 0;

	unsigned char a = (unsigned char) ip;
	unsigned char b = (unsigned char) (ip >> 8);
	unsigned char c = (unsigned char) (ip >> 16);
	unsigned char d = (unsigned char) (ip >> 24);

	sPrintf (ip_addr, "%d.%d.%d.%d", a, b, c, d);

	return 1;
}*/


unsigned net_proto_ipv6_network (net_ipv6 ip)
{
	unsigned short a = (unsigned short) swap16 (ip[0]);

	if (a == 0xfc00 || a == 0xfc02)
		return 0;
	else
		return 1;

	return 0;
}

unsigned net_proto_ipv6_send (netif_t *netif, packet_t *packet, proto_ipv6_t *ip, char *buf, unsigned len)
{
	char *buf_ip = (char *) NEW ((len + 1 + sizeof (proto_ipv6_t)));

	if (!buf_ip)
		return 0;

	unsigned l = sizeof (proto_ipv6_t);

	/* put ip header and buf data to one buffer */
	memcpy (buf_ip, (char *) ip, l);
	memcpy (buf_ip+l, buf, len);

	buf_ip[l+len] = '\0';

	/* send packet */
	unsigned ret = net_packet_send (netif, packet, buf_ip, l+len);

	DEL (buf_ip);

	return ret;
}

unsigned net_proto_ipv6_handler (packet_t *packet, char *buf, unsigned len)
{
	proto_ipv6_t *ip = (proto_ipv6_t *) buf;

	/* ipv6 provide directly length of data - payload length */
	unsigned data_len = swap16 (ip->pl_len);

	//Printf ("ipv6 -> 0x%x\n", ip->nhead);

	switch (ip->nhead) {
		case NET_PROTO_IP_TYPE_TCP6:
			return net_proto_tcp6_handler (packet, ip, buf+sizeof (proto_ipv6_t), data_len);
		case NET_PROTO_IP_TYPE_UDP6:
			return net_proto_udp6_handler (packet, ip, buf+sizeof (proto_ipv6_t), data_len);
		case NET_PROTO_IP_TYPE_ICMP6:
			return net_proto_icmp6_handler (packet, ip, buf+sizeof (proto_ipv6_t), data_len);
	}

	return 0;
}
