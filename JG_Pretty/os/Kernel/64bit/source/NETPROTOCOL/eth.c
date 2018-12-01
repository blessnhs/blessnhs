

#include "eth.h"
#include "if.h"
#include "../Synchronization.h"
#include "../DynamicMemory.h"
#include "../Utility.h"
#include "../console.h"


/* Mutex for data queue */
MUTEX_CREATE (packet_lock);

unsigned char netdev_count = 0;

netdev_t *netdev_create (mac_addr_t addr_mac, unsigned (*read) (char *, unsigned), unsigned (*write) (char *, unsigned), unsigned addr_io)
{
	struct netdev_t *dev;

	/* alloc and init context */
	dev = (struct netdev_t *) NEW (sizeof (struct netdev_t));

	if (!dev)
	{
		Printf("netdev_create new fail \n");
		return 0;
	}

	memcpy (dev->dev_addr, (void *) addr_mac, sizeof (mac_addr_t));


	/* assign ethernet id name */
	dev->name = NEW (sizeof (unsigned char) * 16);

	if (!dev->name) {
		Printf("netdev_create !dev->name fail \n");
		DEL (dev);
		return 0;
	}

	memset(dev->name,0,6);

	memcpy(dev->name,"eth0",4);
	//sPrintf (dev->name, "eth%d", netdev_count);

	dev->base_addr = addr_io;

	dev->read = read;
	dev->write = write;

	dev->info_rx = 0;
	dev->info_tx = 0;

	dev->queue_rx_cnt = 0;
	dev->queue_tx_cnt = 0;

	/* alloc structure for received data queue */
	dev->queue_rx_list.next = &dev->queue_rx_list;
	dev->queue_rx_list.prev = &dev->queue_rx_list;

	dev->queue_tx_list.next = &dev->queue_tx_list;
	dev->queue_tx_list.prev = &dev->queue_tx_list;

	if (!netif_create (dev)) {
		Printf("netdev_create !netif_create fail \n");
		DEL (dev->name);
		DEL (dev);
		return 0;
	}

	netdev_count ++;

	return dev;
}

unsigned netdev_rx_add_queue (struct netdev_t *dev, char *buffer, unsigned len)
{
	if (!dev)
		goto Unlock;

	if(len <= 0)
	{
		goto Unlock;
	}

	Lock(&packet_lock);

	dev->info_rx += len;

	netdev_buffer_queue_t *queue;

	/* alloc and init context */
	queue = (netdev_buffer_queue_t *) NEW (sizeof (netdev_buffer_queue_t));

	if (!queue)
	{
		Unlock(&packet_lock);
		goto Unlock;
	}

	queue->len = len;
	queue->buf = (char *) NEW (sizeof (char) * (len + 1));

	if (!queue->buf) {
		DEL (queue);
		Unlock(&packet_lock);
		goto Unlock;
	}

	memcpy (queue->buf, buffer, len);
	queue->buf[len] = '\0';

	/* add into list */
	queue->next = &dev->queue_rx_list;
	queue->prev = dev->queue_rx_list.prev;
	queue->prev->next = queue;
	queue->next->prev = queue;
	Unlock(&packet_lock);
	return 1;
Unlock:
Unlock(&packet_lock);
	return 0;
}

netdev_buffer_queue_t *netdev_rx_queue (struct netdev_t *dev)
{
	if (!dev)
		return 0;

	netdev_buffer_queue_t *queue;
	for (queue = dev->queue_rx_list.next; queue != &dev->queue_rx_list; queue = queue->next)
	{
		return queue;
	}

	return 0;
}

unsigned netdev_rx_queue_flush (struct netdev_t *dev, netdev_buffer_queue_t *queue)
{
	if (!dev)
		return 0;

	Lock(&packet_lock);

	queue->next->prev = queue->prev;
	queue->prev->next = queue->next;


	DEL (queue->buf);
	DEL (queue);

	Unlock(&packet_lock);
	return 1;
}

int netdev_rx (struct netdev_t *dev, char *buf, unsigned len)
{
	if (!dev)
		return 0;

	/* are there any available data in queue ? */
	netdev_buffer_queue_t *queue = netdev_rx_queue (dev);

	if (!queue)
		return 0;

	unsigned l = queue->len;

	if (l >= len)	/* TODO - received packet is longer then len (> ~MTU) */
		return -1;

	/* copy available data from queue to our buffer */
	memcpy (buf, queue->buf, l);

	/* clean old queue entry */
	netdev_rx_queue_flush (dev, queue);

	/* return available data */
	return l;
}
