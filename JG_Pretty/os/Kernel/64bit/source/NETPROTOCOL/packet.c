/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2009  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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
#include "../task.h"
#include "../Synchronization.h"
#include "../DynamicMemory.h"

MUTEX_CREATE (mutex_packet);

extern netif_t netif_list;
extern unsigned long timer_ticks;

unsigned net_packet_send (netif_t *netif, packet_t *packet, char *buf, unsigned len)
{
	Lock (&mutex_packet);

	if (!netif) {

		Printf("net_packet_send failed \n");
		Unlock (&mutex_packet);
		return 0;
	}

	memcpy (netif->buf_tx, (char *) packet, 14);
	memcpy (netif->buf_tx+14, buf, len);

	netif->dev->write ((char *) netif->buf_tx, 14+len);

	/* add transfered bytes to stats */
	netif->dev->info_tx += 14+len;

	Unlock (&mutex_packet);

	return 1;
}

/* Here we process a received packets */
unsigned net_packet_handler (char *buf, unsigned len)
{
	packet_t *packet = (packet_t *) buf;

	if (!packet)
		return 0;

	int length  = 0;

	int type = (packet->type>>8) | (packet->type<<8);

	if(type < 1500 )
		length = 22;
	else
		length = sizeof(packet_t);

	switch (packet->type) {
		case NET_PACKET_TYPE_IPV4:
			return net_proto_ip_handler (packet, buf+length, len-length);
		case NET_PACKET_TYPE_ARP:
			return net_proto_arp_handler (packet, buf+length, len-length);
		case NET_PACKET_TYPE_IPV6:
			return net_proto_ipv6_handler (packet, buf+length, len-length);
	}

	return 1;
}

/* Task for checking new incoming packets */
unsigned task_net_packet ()
{
		/* let's find network device */
		netif_t *netif;

		/* thread loop */
		while(1)
		{
			for (netif = netif_list.next; netif != &netif_list; netif = netif->next)
			if(netif != 0)
			{
				/* check for incoming data */
				int ret = netdev_rx (netif->dev, netif->buf_rx, 2048);

				/* are there some data ? */
				if (ret) {
					net_packet_handler (netif->buf_rx, ret);

					/* clear buffer */
					memset (netif->buf_rx, 0, 2048);
				}
			}

			Schedule ();
		}

		return 1;
}



unsigned init_packet ()
{
	/* initialize unix domain sockets - we could use it without any network devices */
	if (!init_net_proto_unix ())
	{
		Printf("init_net_proto_unix Fail \n");
		return 0;
	}

	/* setup hostname of the current machine */
	if (!init_hostname ())
	{
		Printf("init_hostname Fail \n");
		return 0;
	}
	
	/* let's find network device first */
//	netif_t *netif = netif_findbyname ("eth0"); /* TODO: all interfaces */

	/* without one ethernet device it does not matter on netstack */
//	if (!netif)
//	{
//		Printf("netif_findbyname Fail \n");
//		return 1;
//	}

	if (!init_net_proto_tcp ())
	{
		Printf("init_net_proto_tcp Fail \n");
		return 0;
	}

	if (!init_net_proto_tcp6 ())
	{
		Printf("init_net_proto_tcp6 Fail \n");
		return 0;
	}

	if (!init_net_proto_udp ())
	{
		Printf("init_net_proto_udp Fail \n");
		return 0;
	}

	if (!init_net_proto_udp6 ())
	{
		Printf("init_net_proto_udp6 Fail \n");
		return 0;
	}

	if (!init_net_proto_dns ())
	{
		Printf("init_net_proto_dns Fail \n");
		return 0;
	}

	if (!init_net_proto_tun6 ())
	{
		Printf("init_net_proto_tun6 Fail \n");
		return 0;
	}

	/* start new task/thread for network core */
	//task_netcore = (task_t *) task_create ("netcore", (unsigned) task_net_packet, 255);
	TCB *task_netcore = CreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_HIGHEST, 0, 0,
	            ( QWORD ) task_net_packet, TASK_LOADBALANCINGID );

	if (!task_netcore)
		return 0;

	return 1;
}
