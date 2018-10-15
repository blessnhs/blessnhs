
#include "eth.h"
#include "net.h"
#include "if.h"
#include "packet.h"
#include "ip.h"
#include "icmp.h"
#include "ndp.h"
#include "tun.h"
#include "checksum.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

static unsigned net_icmp6_ping = 0;

static proto_ipv6_t proto_ipv6_prealloc;
static char proto_icmp_prealloc[sizeof (proto_icmp_t)+65];
static packet_t packet_prealloc;
static char buf_icmp_prealloc[NET_PACKET_MTU + sizeof (proto_icmp_t) + 1];

/* prototype */
unsigned net_proto_icmp6_reply (netif_t *netif, packet_t *packet, proto_ipv6_t *ip, proto_icmp_t *icmp, char *buf, unsigned len);
unsigned net_proto_icmp6_request (netif_t *netif, net_ipv6 dest);

unsigned net_proto_icmp6_ping (netif_t *netif, net_ipv6 ip)
{
	if (!netif)
		return 2;

	unsigned ret = net_proto_icmp6_request (netif, ip);

	if (!ret)
		return 0;

	unsigned long time = GetTickCount();

	while (!net_icmp6_ping) {
		/* timeout 1s - 1000ms */
		if ((GetTickCount()-time) > 1000)
			return 0;

		Schedule ();
	}

	net_icmp6_ping = 0;

	return 1;
}

unsigned net_proto_icmp6_handler (packet_t *packet, proto_ipv6_t *ip, char *buf, unsigned len)
{
	proto_icmp_t *icmp = (proto_icmp_t *) buf;

	if (icmp->type == NET_ICMP6_TYPE_PING_REQUEST) {
		//kPrintf ("ICMP->Request !\n");
	
		/* TODO: melo by to projit vsechny rozhrani */
		netif_t *netif = netif_findbyname ("eth0");
	
		if (net_proto_ipv6_cmp (ip->ip_dest, netif->ipv6))
			return net_proto_icmp6_reply (netif, packet, ip, icmp, buf+8, len-8);

		return 0;
	}

	if (icmp->type == NET_ICMP6_TYPE_PING_REPLY) {
		/* TODO: make it better :) */
		net_icmp6_ping = 1;
		return 1;
	}

	/* ICMPv6 - NDP protocol handler */
	if (icmp->type == NET_NDP_TYPE_NBADVERT) {
		netif_t *netif = netif_findbyname ("eth0");

		proto_ndp_t *ndp = (proto_ndp_t *) buf;
	
		if (net_proto_ipv6_cmp (ip->ip_dest, netif->ipv6))
			return ndp_cache_add (ndp->ll_mac, ndp->target);

		return 0;
	}

	if (icmp->type == NET_NDP_TYPE_NBSOL) {
		netif_t *netif = netif_findbyname ("eth0");

		proto_ndp_t *ndp = (proto_ndp_t *) buf;

		/* send NDP reply */
		if (net_proto_ipv6_cmp (ndp->target, netif->ipv6)) {
			ndp_cache_add (ndp->ll_mac, ip->ip_source);
			return ndp_send_nbreply (netif, ndp->ll_mac, ip->ip_source);
		}
	}

	if (icmp->type == NET_NDP_TYPE_RADVERT) {
		netif_t *netif = netif_findbyname ("eth0");

		proto_icmp_ndp2_t *ndp = (proto_icmp_ndp2_t *) buf;

		/* send NDP reply */
		return ndp_send_rreply (netif, ndp, ip->ip_source, ip->ip_dest);
		
	}

	return 0;
}

unsigned net_proto_icmp6_request (netif_t *netif, net_ipv6 dest)
{
	mac_addr_t mac_dest;

	if (!tun6_addr_get ()) {
		unsigned get = ndp_cache_get (dest, &mac_dest);
	
		if (!get) {
			ndp_send_nbrequest (netif, dest);
	
			unsigned i = 0;
			/* 200ms for waiting on NDP reply */
			while (i < 200) {
				get = ndp_cache_get (dest, &mac_dest);
	
				if (get)
					break;
	
				/* TODO: make better waiting for ARP reply */
				Sleep (1000);
	
				Schedule ();
		
				i ++;
			}
	
			if (!get)
				return 0;
		}
	} else {
		/* IPv6 tunnel stuff */
		net_ipv4 tunnel = tun6_addr_get ();
		unsigned get = arp_cache_get (tunnel, &mac_dest);
	
		if (!get) {
			arp_send_request (netif, tunnel);
	
			unsigned i = 0;
			/* 100ms for waiting on ARP reply */
			while (i < 100) {
				get = arp_cache_get (tunnel, &mac_dest);
	
				if (get)
					break;
	
				/* TODO: make better waiting for ARP reply */
				Sleep (1000);
	
				Schedule ();
		
				i ++;
			}
	
			if (!get)
				return 0;
		}
	}

	/* packet header */
	packet_t *packet = (packet_t *) &packet_prealloc;

	if (!packet)
		return 0;

	memcpy (&packet->mac_source, netif->dev->dev_addr, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);
	packet->type = NET_PACKET_TYPE_IPV6;

	/* ip layer */
	proto_ipv6_t *ip = (proto_ipv6_t *) &proto_ipv6_prealloc;

	if (!ip)
		return 0;

	/* there are some fixed values - yeah it is horrible */
	ip->ver = 0x60;
	ip->tclass = 0x0;
	ip->flabel = 0x0;
	ip->pl_len = swap16 (64);
	ip->nhead = NET_PROTO_IP_TYPE_ICMP6;
	ip->hop = 0xff;
	memcpy (ip->ip_source, (void *) netif->ipv6, sizeof (net_ipv6));
	memcpy (ip->ip_dest, (void *) dest, sizeof (net_ipv6));

	/* icmp layer */
	proto_icmp_t *icmp = (proto_icmp_t *) &proto_icmp_prealloc;

	if (!icmp)
		return 0;

	memset ((char *) icmp, 0, 64);

	icmp->type = NET_ICMP6_TYPE_PING_REQUEST;
	icmp->code = 0;
	icmp->seq = 0x3ff3;

	unsigned ret = 0;

	icmp->checksum = checksum16_ipv6 (ip->ip_source, ip->ip_dest, icmp, sizeof (proto_icmp_t)+56, NET_PROTO_IP_TYPE_ICMP6);
	
	if (!tun6_addr_get ())
		ret = net_proto_ipv6_send (netif, packet, ip, (char *) icmp, sizeof (proto_icmp_t)+56);
	else	/* tunnel mode */
		ret = net_proto_tun6_send (netif, packet, ip, (char *) icmp, sizeof (proto_icmp_t)+56);

	return ret;
}

unsigned net_proto_icmp6_reply (netif_t *netif, packet_t *packet, proto_ipv6_t *ip, proto_icmp_t *icmp, char *buf, unsigned len)
{
	mac_addr_t mac_dest;
	mac_addr_t mac_source;

	/* cross mac and ip address, because this packet have to go back */
	memcpy (&mac_dest, packet->mac_source, 6);
	memcpy (&mac_source, packet->mac_dest, 6);

	/* assign new values to old structures */
	memcpy (&packet->mac_source, mac_source, 6);
	memcpy (&packet->mac_dest, mac_dest, 6);

	/* TODO: identification, flags, etc */

	/* ip layer */
	net_ipv6 tmp;
	memcpy (tmp, (void *) ip->ip_dest, sizeof (net_ipv6));

	memcpy (ip->ip_dest, (void *) ip->ip_source, sizeof (net_ipv6));
	memcpy (ip->ip_source, (void *) tmp, sizeof (net_ipv6));

	icmp->type = NET_ICMP6_TYPE_PING_REPLY;
	icmp->checksum = 0;

	char *buf_icmp = (char *) &buf_icmp_prealloc;

	if (!buf_icmp)
		return 0;

	unsigned l = sizeof (proto_icmp_t);

	memcpy (buf_icmp, (char *) icmp, l);
	memcpy (buf_icmp+l, buf, len);

	buf_icmp[l+len] = '\0';

	unsigned ret = 0;

	/* calculate checksum and put it to icmp header */
	proto_icmp_t *icmp_ = (proto_icmp_t *) buf_icmp;

	icmp_->checksum = checksum16_ipv6 (ip->ip_source, ip->ip_dest, buf_icmp, len+l, NET_PROTO_IP_TYPE_ICMP6);

	if (!tun6_addr_get ())
		ret = net_proto_ipv6_send (netif, packet, ip, buf_icmp, len+l);
	else
		ret = net_proto_tun6_send (netif, packet, ip, buf_icmp, len+l);

	return ret;
}

