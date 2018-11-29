

#include "eth.h"
#include "net.h"
#include "if.h"
#include "ip.h"
#include "packet.h"
#include "dns.h"
#include "socket.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

dns_cache_t dns_cache_list;

net_ipv4 dns_ip;

unsigned short dns_trans = NULL;

#define DEFAULT_DNS_ADDRESS	NET_IPV4_TO_ADDR (208,67,222,222)

/* prototype */
int dns_cache_add (char *hostname, unsigned char len, void *ip, unsigned char type);


unsigned dns_addr (net_ipv4 dns)
{
	dns_ip = dns;

	return 1;
}

net_ipv4 dns_addr_get ()
{
	return dns_ip;
}

int dns_send_request (char *hostname, void *ip, unsigned char type)
{
	if (!ip || !hostname)
			return -1;

		if (!type)
			return 0;

		int sock = 0;
		int err = 0;

		/* Create a socket */
		if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
			Printf("Cant create socket");
			return -1;
		}

		sockaddr_in serverSock;

		serverSock.sin_family = AF_INET;
		serverSock.sin_port = htons (DEFAULT_DNS_PORT);
		memcpy (&(serverSock.sin_addr), &dns_ip, sizeof (net_ipv4));

		/* Lets connect to dns server */
		if (connect (sock, (sockaddr *) &serverSock, sizeof (serverSock)) == -1) {
			Printf("Connection cant be estabilished");
			return -1;
		}

		unsigned short len = strlen (hostname);

		proto_dns_t *dns = (proto_dns_t *) NEW (sizeof (proto_dns_t)+len+7);

		if (!dns)
			return -1;

		/* dns header */
		dns->trans = swap16 (dns_trans ++);
		dns->flags = swap16 (0x0100);
		dns->question = swap16 (1);
		dns->answer = 0;
		dns->auth = 0;
		dns->add = 0;

		char *dns_ = (char *) dns;

		/* dns queries */
		// correct - dsl.cz 	: 03 64 73 6c 02 63 7a 00
		// incorrect - dsl.cz	: 06 64 73 6c 2e 63 7a 00

		unsigned short dtype = (type == sizeof (net_ipv6) ? swap16 (0x1c) : swap16 (1));
		unsigned short class = swap16 (1);

		char *name_coded = (char *) NEW (sizeof (char) * (len + 2));

		if (!name_coded)
			return -1;

		memcpy (name_coded+1, hostname, len);
		name_coded[len+1] = '\0';

		unsigned i = 1;
		unsigned y = 0;
		unsigned z = 0;

		while (i < len+2) {
			if (name_coded[i] == '.' || name_coded[i] == '\0') {
				if (y == 0)
					name_coded[0] = i-1;
				else
					name_coded[y] = z-1;

				y = i;
				z = 0;
			}

			i ++;
			z ++;
		}

		/* setup dns query */
		memcpy (dns_+12, name_coded, len+2);
		memcpy (dns_+14+len, (char *) &dtype, 2);
		memcpy (dns_+16+len, (char *) &class, 2);

		DEL (name_coded);

		int ret = send (sock, (char *) dns, sizeof (proto_dns_t)+len+6, 0);

		/* it is not done */
		char buf[512];

		/* receive response from dns server */
		ret = recv (sock, buf, 512, 0);

		char target[32];

		if (type >= 32)
			return 0;

		/* dns server send respond */
		if (ret) {
			proto_dns_t *dns_res = (proto_dns_t *) buf;

			/* ipv4 and ipv6 capable */
			if (type == 4 || type == 16) {
				if (dns_res->flags == 0x8081) {	/* all is ok, no error */
					proto_dns_answer_t *dns_answer = (proto_dns_answer_t *) ((char *) buf + 18 + len);
					unsigned next = 0;

					/* walk trough whole answer list */
					for (;;) {
						next += sizeof (proto_dns_answer_t) + swap16 (dns_answer->dlen);

						if (!dns_answer->type || !dns_answer->aclass) {
							err ++;
							break;
						}

						if (type == 16 && dns_answer->type == 0x1c00) {	/* is it AAAA answer ? */
							memcpy (&target, (void *) dns_answer + sizeof (proto_dns_answer_t), type);

							dns_cache_add (hostname, len, (void *) target, type);
							break;
						} else if (type == 4 && dns_answer->type == 0x100) {	/* is it A answer ? */
							memcpy (&target, (void *) dns_answer + sizeof (proto_dns_answer_t), type);

							dns_cache_add (hostname, len, (void *) target, type);
							break;
						} else
							dns_answer = (proto_dns_answer_t *) ((char *) dns_answer + next);
					}
				} else
					err ++;
			} else
				err ++;
		}

		sclose (sock);

		DEL (dns);

		memcpy (ip, target, type);
		return err ? 0 : 1;
}


/* DNS Cache */
int dns_cache_add (char *hostname, unsigned char len, void *ip, unsigned char type)
{
	if (!ip || !hostname)
		return -1;

	if (!type)
		return 0;

	dns_cache_t *cache;
	for (cache = dns_cache_list.next; cache != &dns_cache_list; cache = cache->next) {
		if (!strcmp (cache->hostname, hostname))
			return 0;
	}

	/* alloc and init context */
	cache = (dns_cache_t *) NEW (sizeof (dns_cache_t));

	if (!cache)
		return 0;

	cache->type = type;

	cache->ip = NEW (type);

	if (!cache->ip) {
		DEL (cache);
		return 0;
	}

	memcpy (cache->ip, ip, type);

	cache->len = len;

	cache->hostname = (char *) NEW (sizeof (char) * (len + 1));

	if (!cache->hostname) {
		DEL (cache->ip);
		DEL (cache);
		return 0;
	}

	memcpy (cache->hostname, hostname, len);
	cache->hostname[len] = '\0';

	/* add into list */
	cache->next = &dns_cache_list;
	cache->prev = dns_cache_list.prev;
	cache->prev->next = cache;
	cache->next->prev = cache;
	return 1;
}

int dns_cache_get (char *hostname, void *ip, unsigned char type)
{
	if (!ip || !hostname)
	{
		Printf("dns_cache_get return %s %s\n",ip,hostname);
		return -1;
	}

	dns_cache_t *cache;

	for (cache = dns_cache_list.next; cache != &dns_cache_list; cache = cache->next)
	{
		if (cache->type == type)
		{
			if (!strcmp (cache->hostname, hostname))
			{
				memcpy (ip, cache->ip, cache->type);
				return 1;
			}
		}
	}

	return 0;
}

unsigned init_net_proto_dns ()
{
	dns_cache_list.next = &dns_cache_list;
	dns_cache_list.prev = &dns_cache_list;

	dns_trans = 1024;

	dns_addr (DEFAULT_DNS_ADDRESS);

	return 1;
}
