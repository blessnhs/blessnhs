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
#include "../utility.h"

static char buf_ip_prealloc[NET_PACKET_MTU + sizeof (net_ipv4) + 1];

void net_proto_ip_print (net_ipv4 ip)
{
	unsigned char a = (unsigned char) ip;
	unsigned char b = (unsigned char) (ip >> 8);
	unsigned char c = (unsigned char) (ip >> 16);
	unsigned char d = (unsigned char) (ip >> 24);

	Printf ("%d.%d.%d.%d\n", a, b, c, d);
}

net_ipv4 net_proto_ip_convert (char *ip)
{
	if (!ip)
		return 0;

	unsigned char a = 0;
	unsigned char b = 0;
	unsigned char c = 0;
	unsigned char d = 0;

	unsigned g = 0;
	unsigned i = 0;
	unsigned y = strlen (ip);

	if (!y)
		return 0;

	char *str = (char *) NEW (sizeof (char) * (y + 1));

	if (!str)
		return 0;

	memcpy (str, ip, y);
	str[y] = '\0';

	unsigned h[4];

	while (i < y) {
		if (str[i] == '.') {
			str[i] = '\0';
			h[g] = i+1;
			g ++;
		}

		i ++;
	}

	if (g != 3) {
		DEL (str);
		return 0;
	}
	
	a = atoi (str);
	b = atoi (str+h[0]);
	c = atoi (str+h[1]);
	d = atoi (str+h[2]);

	DEL (str);

	return NET_IPV4_TO_ADDR (a, b, c, d);
}

unsigned net_proto_ip_convert2 (net_ipv4 ip, char *ip_addr)
{
	if (!ip_addr)
		return 0;

	unsigned char a = (unsigned char) ip;
	unsigned char b = (unsigned char) (ip >> 8);
	unsigned char c = (unsigned char) (ip >> 16);
	unsigned char d = (unsigned char) (ip >> 24);

	SPrintf (ip_addr, "%d.%d.%d.%d", a, b, c, d);

	return 1;
}

unsigned net_proto_ip_network (net_ipv4 ip)
{
	unsigned char a = (unsigned char) ip;

	if (a == 192 || a == 10 || a == 127 || a == 255) /* TODO: Dynamic */
		return 0;
	else
		return 1;

	return 0;
}

unsigned net_proto_ip_send (netif_t *netif, packet_t *packet, proto_ip_t *ip, char *buf, unsigned len)
{
	if (len +1 + sizeof (proto_ip_t) > NET_PACKET_MTU + sizeof (net_ipv4)) {
		Printf ("ERROR -> IP packet is too long (%d/%d Bytes)\n", len + sizeof (proto_ip_t), NET_PACKET_MTU + sizeof (net_ipv4));
		return 0;
	}
  
	char *buf_ip = (char *) &buf_ip_prealloc; //(char *) NEW ((len + 1 + sizeof (proto_ip_t)));

	if (!buf_ip)
		return 0;

	unsigned l = sizeof (proto_ip_t);

	/* 16bit IP header checksum */
	ip->checksum = checksum16 (ip, l);

	/* put ip header and buf data to one buffer */
	memcpy (buf_ip, (char *) ip, l);
	memcpy (buf_ip+l, buf, len);

	/* send packet */
	unsigned ret = net_packet_send (netif, packet, buf_ip, l+len);

	//DEL (buf_ip);

	return ret;
}

unsigned net_proto_ip_handler (packet_t *packet, char *buf, unsigned len)
{
	proto_ip_t *ip = (proto_ip_t *) buf;

	/* calculate real length of header */
	unsigned head_len = ip->head_len * 4;

	switch (ip->proto) {
		case NET_PROTO_IP_TYPE_TCP:
		{
			HandleIPPacket((ip->ip_source),
						(ip->ip_dest),
						(uint8_t*)buf + head_len,
						ntohs(ip->total_len) - head_len);
			return;
		}
		case NET_PROTO_IP_TYPE_UDP:
			return net_proto_udp_handler (packet, ip, buf+head_len, len-head_len);
		case NET_PROTO_IP_TYPE_ICMP:
			return net_proto_icmp_handler (packet, ip, buf+head_len, len-head_len);
#ifdef CONFIG_PROTO_TUN6
		case NET_PROTO_IP_TYPE_TUN6:
			return net_proto_tun6_handler (packet, ip, buf+head_len, len-head_len);
#endif
	}

	return 0;
}


unsigned inet_addr (const char *src)
{
	if (!src)
		return 0;

	unsigned char a = 0;
	unsigned char b = 0;
	unsigned char c = 0;
	unsigned char d = 0;

	unsigned g = 0;
	unsigned i = 0;
	unsigned y = strlen (src);

	if (!y)
		return 0;

	char *str = (char *) NEW (sizeof (char) * (y + 1));

	if (!str)
		return 0;

	memcpy (str, src, y);
	str[y] = '\0';

	unsigned h[4];

	while (i < y) {
		if (str[i] == '.') {
			str[i] = '\0';
			h[g] = i+1;
			g ++;
		}
	}

	if (g != 3) {
		DEL (str);
		return -1;
	}

	a = atoi (str);
	b = atoi (str+h[0]);
	c = atoi (str+h[1]);
	d = atoi (str+h[2]);

	DEL (str);

	g = NET_IPV4_TO_ADDR (a, b, c, d);

	return (unsigned) g;
}

