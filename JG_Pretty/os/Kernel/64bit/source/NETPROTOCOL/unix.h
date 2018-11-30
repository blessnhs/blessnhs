/*
 *  ZeX/OS
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

#ifndef _UNIX_H
#define _UNIX_H

#include "ip.h"

#define PROTO_UNIX_CONN_STATE_ESTABILISHED	0x1

/* Unix Domain connection structure */
typedef struct proto_unix_conn_context {
	struct proto_unix_conn_context *next, *prev;

	char *path;

	unsigned short flags;

	unsigned short state;

	unsigned char bind;

	unsigned short fd;

	unsigned len;
	char *data;

	void *session;
} proto_unix_conn_t;

/* backlog for accept */
typedef struct proto_unix_backlog_context {
	struct proto_unix_backlog_context *next, *prev;

	proto_unix_conn_t *conn;
	proto_unix_conn_t *session;
} proto_unix_backlog_t;

int net_proto_unix_read_cache (proto_unix_conn_t *conn, char *data, unsigned len);
int net_proto_unix_write (proto_unix_conn_t *conn, char *data, unsigned len);
proto_unix_conn_t *net_proto_unix_conn_find (int fd);
proto_unix_conn_t *net_proto_unix_conn_findbypath (char *path);
int net_proto_unix_conn_set (proto_unix_conn_t *conn, char *path, unsigned char bind);
unsigned net_proto_unix_conn_del (proto_unix_conn_t *conn);
int net_proto_unix_conn_add ();
int net_proto_unix_backlog_add (proto_unix_conn_t *conn, proto_unix_conn_t *session);

/* externs */
extern unsigned init_net_proto_unix ();

#endif
