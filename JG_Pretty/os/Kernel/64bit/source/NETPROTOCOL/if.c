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
#include "ip.h"
#include "tun.h"
#include "../time.h"
#include "../DynamicMemory.h"
#include "../utility.h"

netif_t netif_list;

static net_port net_port_curr;

#define DEFAULT_IP_ADDRESS	NET_IPV4_TO_ADDR (192,168,0,50)
#define DEFAULT_GW_ADDRESS	NET_IPV4_TO_ADDR (192,168,0,1)

/* type of the address config in text form */
static char *if_cfg_type[] = { "Static", "DHCP", "Advert" };

netif_t *netif_findbyname (char *name)
{
	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next)
	{
		if(netif == 0)
		{
			continue;
		}

		if (!strcmp (netif->dev->name, name))
			return netif;

	}

	return 0;
}

unsigned netif_ip_addr (netif_t *netif, net_ipv4 ip, unsigned char cfg)
{
	if (!netif)
		return 0;

	netif->ip = ip;
	netif->cfg = cfg;

	return 1;
}

unsigned netif_ipv6_addr (netif_t *netif, net_ipv6 ip, unsigned char cfg)
{
	if (!netif)
		return 0;

	memcpy (netif->ipv6, ip, sizeof (net_ipv6));
	netif->cfgv6 = cfg;

	return 1;
}

unsigned netif_gw_addr (netif_t *netif, net_ipv4 gw)
{
	if (!netif)
		return 0;

	netif->gw = gw;

	return 1;
}

unsigned netif_gwv6_addr (netif_t *netif, net_ipv6 gw)
{
	if (!netif)
		return 0;

	memcpy (netif->gwv6, gw, sizeof (net_ipv6));

	return 1;
}

netif_t *netif_create (struct netdev_t *dev)
{
	if (!dev)
	{
		Printf("netif_create !dev fail \n");
		return 0;
	}

	netif_t *netif;

	/* alloc and init context */
	netif = (netif_t *) NEW (sizeof (netif_t));

	if (!netif)
	{
		Printf("netif_create !netif fail \n");
		return 0;
	}

	netif->dev = dev;

	if (!dev->dev_addr)
	{
		Printf("netif_create !dev_addr fail \n");
		return 0;
	}

	netif_ip_addr (netif, DEFAULT_IP_ADDRESS, IF_CFG_TYPE_STATIC);
	netif_gw_addr (netif, DEFAULT_GW_ADDRESS);

	net_ipv6 ipv6;
	NET_IPV6_TO_ADDR (ipv6, 0xfc00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10);
	netif_ipv6_addr (netif, ipv6, IF_CFG_TYPE_STATIC);

	NET_IPV6_TO_ADDR (ipv6, 0xfc00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1);
	netif_gwv6_addr (netif, ipv6);

	/* add into list */
	netif->next = &netif_list;
	netif->prev = netif_list.prev;
	netif->prev->next = netif;
	netif->next->prev = netif;

	return netif;
}

net_port netif_port_get ()
{
	return net_port_curr ++;
}

void iflist_display ()
{
	netif_t *netif;
	for (netif = netif_list.next; netif != &netif_list; netif = netif->next) {
		if (!netif)
			continue;

		Printf ("if: %s - MAC addr: %x:%x:%x:%x:%x:%x\n\tRx bytes: %lu\tTx bytes: %d\n", netif->dev->name,
		netif->dev->dev_addr[0], netif->dev->dev_addr[1], netif->dev->dev_addr[2],
		netif->dev->dev_addr[3], netif->dev->dev_addr[4], netif->dev->dev_addr[5],
		netif->dev->info_rx, netif->dev->info_tx);
		Printf ("\tEthernet I/O base:\t0x%x\n", netif->dev->base_addr);
		Printf ("\tEthernet IP address:\t"); net_proto_ip_print (netif->ip); Printf (" /%s", if_cfg_type[netif->cfg]);
		Printf ("\n\t\t\t\t"); net_proto_ipv6_print (netif->ipv6); Printf (" /%s", if_cfg_type[netif->cfgv6]);
		Printf ("\n\tGateway IP address:\t"); net_proto_ip_print (netif->gw);
		Printf ("\n\t\t\t\t"); net_proto_ipv6_print (netif->gwv6);
		Printf ("\nName : %s",netif->dev->name);
		if (tun6_addr_get ()) {
			Printf ("\n\tTunnel IP address:\t::"); net_proto_ip_print (tun6_addr_get ());
		}

		Printf ("\n");
	}
}

unsigned netif_read (struct netdev_t *dev, char *buf)
{

	return 1;
}

unsigned init_netif ()
{
	netif_list.next = &netif_list;
	netif_list.prev = &netif_list;

	if (!init_proto_arp ())
	{
		Printf ("\n\init_proto_arp  fail");
		return 0;
	}

	if (!init_proto_ndp ())
	{
		Printf ("\n\tinit_proto_ndp fail");
		return 0;
	}
	
	/* preset client-side port for new connections */
/*	tm *t = rtc_getcurrtime ();
	
	if (t)
		net_port_curr = (net_port) (t->__tm_gmtoff & 0xfff + 1024);
	else*/
		net_port_curr = 1024;
	
	return 1;
}
