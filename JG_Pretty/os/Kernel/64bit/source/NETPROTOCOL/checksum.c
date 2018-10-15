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

#include "if.h"
#include "eth.h"
#include "packet.h"
#include "../Utility.h"

static char buf_prealloc[NET_PACKET_MTU + 64];

unsigned short checksum16_ones (unsigned sum, const void *_buf, int len)
{
	const unsigned short *buf = _buf;

	while (len >= 2) {
		sum += *buf ++;

		if (sum & 0x80000000)
			sum = (sum & 0xffff) + (sum >> 16);

		len -= 2;
	}

	if (len) {
		unsigned char temp[2];

		temp[0] = *(unsigned char *) buf;
		temp[1] = 0;

		sum += *(unsigned short *) temp;
	}

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return sum;
}

unsigned short checksum16 (void *_buf, int len)
{
	return ~checksum16_ones (0, _buf, len);
}

unsigned short checksum16_2 (void *buf1, int len1, void *buf2, int len2)
{
	unsigned sum;

	sum = checksum16_ones (0, buf1, len1);

	return ~checksum16_ones (sum, buf2, len2);
}

/* pseudo-header for tcp checksum */
typedef struct proto_tcp_pseudo_t {
	unsigned ip_source;
	unsigned ip_dest;	
	unsigned char zero;
	unsigned char proto;
	unsigned short tcp_len;
} proto_tcp_pseudo_t;

unsigned short checksum16_tcp (unsigned ip_source, unsigned ip_dest, char *buf, unsigned short len)
{
	char *buf_pseudo = (char *) &buf_prealloc;

	if (!buf_pseudo)
		return 0;

	proto_tcp_pseudo_t *pseudo = (proto_tcp_pseudo_t *) buf_pseudo;

	pseudo->ip_source = ip_source;
	pseudo->ip_dest = ip_dest;
	pseudo->tcp_len = swap16 (len);
	pseudo->proto = 0x6;
	pseudo->zero = 0x0;

	memcpy (buf_pseudo+12, buf, len);

	buf_pseudo[len+12] = '\0';

	unsigned short ret = checksum16 (buf_pseudo, len+12);

	return ret;
}

unsigned short checksum16_udp (unsigned ip_source, unsigned ip_dest, char *buf, unsigned short len)
{
	char *buf_pseudo = (char *) &buf_prealloc;

	if (!buf_pseudo)
		return 0;

	proto_tcp_pseudo_t *pseudo = (proto_tcp_pseudo_t *) buf_pseudo;

	pseudo->ip_source = ip_source;
	pseudo->ip_dest = ip_dest;
	pseudo->tcp_len = swap16 (len);
	pseudo->proto = 0x11;
	pseudo->zero = 0x0;

	memcpy (buf_pseudo+12, buf, len);

	buf_pseudo[len+12] = '\0';

	unsigned short ret = checksum16 (buf_pseudo, len+12);

	return ret;
}

/* pseudo-header for icmp6 checksum */
typedef struct proto_ipv6_pseudo_t {
	unsigned short ip_source[8];
	unsigned short ip_dest[8];
	unsigned pl_len;
	unsigned zero:24;
	unsigned char next_head;
} proto_ipv6_pseudo_t;

unsigned short checksum16_ipv6 (unsigned short ip_source[8], unsigned short ip_dest[8], char *buf, unsigned len, unsigned char proto)
{
	char *buf_pseudo = (char *) &buf_prealloc;

	if (!buf_pseudo)
		return 0;

	proto_ipv6_pseudo_t *pseudo = (proto_ipv6_pseudo_t *) buf_pseudo;

	memcpy (pseudo->ip_source, ip_source, 16);
	memcpy (pseudo->ip_dest, ip_dest, 16);

	pseudo->pl_len = swap32 (len);
	pseudo->zero = 0x0;
	pseudo->next_head = proto;

	//Printf ("ipv6 source -> %x:%x:::\n", swap16 (pseudo->ip_source[0]), swap16 (pseudo->ip_source[1]));
	//Printf ("ipv6 dest -> %x:%x:::\n", swap16 (pseudo->ip_dest[0]), swap16 (pseudo->ip_dest[1]));
	//Printf ("proto_ipv6_pseudo_t size: %d\n", sizeof (proto_ipv6_pseudo_t));

	memcpy (buf_pseudo+40, buf, len);

	buf_pseudo[len+40] = '\0';

	unsigned short ret = checksum16 (buf_pseudo, len+40);

	return ret;
}
