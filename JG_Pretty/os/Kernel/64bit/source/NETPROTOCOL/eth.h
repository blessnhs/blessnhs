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

#ifndef _ETH_H
#define _ETH_H

#define PROT_TYPE_IPV4 0x0800
#define PROT_TYPE_ARP  0x0806

#define ETHERNET_HEADER_SIZE (6+6+2)
#define ETHERNET_MAX_SIZE (ETHERNET_HEADER_SIZE+1500)
#define ETHERNET_MIN_SIZE (ETHERNET_HEADER_SIZE+46)

typedef unsigned char mac_addr_t[6];

typedef struct netdev_buffer_queue_context {
	struct netdev_buffer_queue_context *next, *prev;

	unsigned short len;
	char *buf;
} netdev_buffer_queue_t;


typedef struct netdev_t {
	mac_addr_t dev_addr;
	unsigned long base_addr;
	void *priv;
	unsigned char irq;
	char *name;

	/* main functions, what works with ethernet directly */
	unsigned (*read) (char *, unsigned);
	unsigned (*write) (char *, unsigned);

	/* some info about traffic */
	unsigned long info_rx;
	unsigned long info_tx;

	/* actual queue count */
	int queue_rx_cnt;
	int queue_tx_cnt;

	/* queue for received/transfered data */
	netdev_buffer_queue_t queue_rx_list;
	netdev_buffer_queue_t queue_tx_list;
} netdev_t;


extern netdev_t *netdev_create (mac_addr_t addr_mac, unsigned (*read) (char *, unsigned), unsigned (*write) (char *, unsigned), unsigned addr_io);
extern unsigned netdev_rx_add_queue (struct netdev_t *dev, char *buffer, unsigned len);
extern netdev_buffer_queue_t *netdev_rx_queue (struct netdev_t *dev);
extern unsigned netdev_rx_queue_flush (struct netdev_t *dev, netdev_buffer_queue_t *queue);
extern int netdev_rx (struct netdev_t *dev, char *buf, unsigned len);

#endif
