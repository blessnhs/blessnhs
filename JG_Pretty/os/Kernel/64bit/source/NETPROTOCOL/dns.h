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
