#ifndef _DNS_H
#define _DNS_H

#define DEFAULT_DNS_PORT	53

#define	__PACKED__ __attribute__ ((__packed__))

typedef struct dns_cache_context {
	struct dns_cache_context *next, *prev;

	unsigned char type;
	void *ip;
	unsigned char len;
	char *hostname;
} dns_cache_t;

typedef struct proto_dns_answer_t {
	unsigned short name;
	unsigned short type;
	unsigned short aclass;
	unsigned ttl;
	unsigned short dlen;
} __PACKED__ proto_dns_answer_t;

typedef struct proto_dns_t {
	unsigned short trans;
	unsigned short flags;
	unsigned short question;
	unsigned short answer;
	unsigned short auth;
	unsigned short add;
} __PACKED__ proto_dns_t;

/* externs */
extern int dns_send_request (char *hostname, void *ip, unsigned char type);
extern int dns_cache_add (char *hostname, unsigned char len, void *ip, unsigned char type);
extern int dns_cache_get (char *hostname, void *ip, unsigned char type);
extern unsigned init_net_proto_dns ();

#endif
