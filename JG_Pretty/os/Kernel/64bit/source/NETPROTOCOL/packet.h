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


#ifndef _PACKET_H
#define _PACKET_H

#define NET_PACKET_MTU		1500

#define NET_PACKET_TYPE_IPV6	0xdd86
#define NET_PACKET_TYPE_IPV4	0x8
#define NET_PACKET_TYPE_STP	0x2600
#define NET_PACKET_TYPE_ARP	0x608

typedef struct packet_t {
	mac_addr_t mac_dest;
	mac_addr_t mac_source;
	unsigned short type;
}  __attribute__((packed))  packet_t;

unsigned net_packet_send (netif_t *netif, packet_t *packet, char *buf, unsigned len);
unsigned net_packet_send2 (netif_t *netif, char *buf, unsigned len);
unsigned init_packet ();
unsigned net_packet_handler(char *buf, unsigned len);
#endif
