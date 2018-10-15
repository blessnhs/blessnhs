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

#ifndef _IF_H
#define _IF_H

#include "eth.h"
#include "net.h"
#include "ip.h"

/* type of the ip adddress configuration */
#define IF_CFG_TYPE_STATIC	0
#define IF_CFG_TYPE_DHCP	1
#define IF_CFG_TYPE_RADVERT	2	/* ipv6 only */

/* Network interface structure */
typedef struct netif_context {
	struct netif_context *next, *prev;

	//struct netaddr_t *addr;
	struct netdev_t *dev;
	net_ipv4 ip;
	net_ipv4 gw;
	net_ipv6 ipv6;
	net_ipv6 gwv6;
	char buf_rx[2048 * 2];
	char buf_tx[2048 * 2];
	unsigned char cfg;
	unsigned char cfgv6;
} netif_t;

extern unsigned netif_ip_addr (netif_t *netif, net_ipv4 ip, unsigned char cfg);
extern unsigned netif_ipv6_addr (netif_t *netif, net_ipv6 ip, unsigned char cfg);
extern unsigned netif_gw_addr (netif_t *netif, net_ipv4 gw);
extern unsigned netif_gwv6_addr (netif_t *netif, net_ipv6 gw);
extern netif_t *netif_findbyname (char *name);
extern netif_t *netif_create (struct netdev_t *dev);
extern void iflist_display ();
extern net_port netif_port_get ();
extern unsigned init_netif ();

#endif
