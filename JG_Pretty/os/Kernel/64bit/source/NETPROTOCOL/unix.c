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

#include "file.h"
#include "fd.h"
#include "net.h"
#include "if.h"
#include "packet.h"
#include "socket.h"
#include "unix.h"
#include "../errno.h"
#include "../DynamicMemory.h"
#include "../task.h"

proto_unix_conn_t proto_unix_conn_list;
proto_unix_backlog_t proto_unix_backlog_list;

/* prototypes */
int net_proto_unix_read_cache (proto_unix_conn_t *conn, char *data, unsigned len);
int net_proto_unix_write (proto_unix_conn_t *conn, char *data, unsigned len);
proto_unix_conn_t *net_proto_unix_conn_find (int fd);
proto_unix_conn_t *net_proto_unix_conn_findbypath (char *path);
int net_proto_unix_conn_set (proto_unix_conn_t *conn, char *path, unsigned char bind);
unsigned net_proto_unix_conn_del (proto_unix_conn_t *conn);
int net_proto_unix_conn_add ();
int net_proto_unix_backlog_add (proto_unix_conn_t *conn, proto_unix_conn_t *session);

/** Unix protocol
 *  User-friendly socket functions
 */
int net_proto_unix_socket (fd_t *fd)
{
	return net_proto_unix_conn_add (fd);
}

int net_proto_unix_connect (int fd, sockaddr_un *addr)
{
	int ret = -1;

	proto_unix_conn_t *session = net_proto_unix_conn_findbypath (addr->sun_path);

	if (!session)
		return -1;

	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	ret = net_proto_unix_conn_set (conn, addr->sun_path, 0);

	if (!net_proto_unix_backlog_add (session, conn))
		return -1;

	//conn->session = session;

	unsigned long stime = GetTickCount();

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		if (!ret)
			return -1;
	
		for (;;) {
			Schedule ();

			/* timeout for 3s */
			if ((stime+3000) < GetTickCount())
				return -1;

			if (conn->state == PROTO_UNIX_CONN_STATE_ESTABILISHED) {
				ret = 0;
				break;
			}
		}
	} else
	/* non-blocking mode */
		return -1;

	return ret;
}

int net_proto_unix_send (int fd, char *msg, unsigned size)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_unix_write (conn, msg, size);

	if (ret) {
		/* blocking mode */
		if (!(conn->flags & O_NONBLOCK)) {
			
		} else {
		/* non-blocking mode */
		/* TODO: ? */
			
		}
	}

	return size;
}

int net_proto_unix_recv (int fd, char *msg, unsigned size)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -3;

	if (!msg)
		return -4;

	int ret = 0;

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		unsigned long stime = GetTickCount();

		while (!conn->len) {
			if ((stime+10000) < GetTickCount())
				return 0;

			Schedule ();
		}
	} else {
		if (!conn->len)
			return 0;
	}

	if (!conn->data)
		return -5;

	if (conn->len >= size)
		return -2;

	ret = conn->len;

	if (conn->len > 0)
		memcpy (msg, conn->data, conn->len);

	conn->len = 0;

	DEL (conn->data);

	return ret;
}


int net_proto_unix_close (int fd)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_unix_conn_del (conn);

	return ret;
}

int net_proto_unix_fcntl (int fd, int cmd, long arg)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	switch (cmd) {
		case F_GETFL:
			return conn->flags;
		case F_SETFL:
			conn->flags = arg;
			return 0;
	}

	return -1;
}

int net_proto_unix_bind (int fd, sockaddr_un *addr, socklen_t len)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	int ret = net_proto_unix_conn_set (conn, addr->sun_path, 1);

	if (ret)
		return 0;

	return -1;
}

int net_proto_unix_listen (int fd, int backlog)
{
	/* TODO */

	return 0;
}

int net_proto_unix_accept (int fd, sockaddr_un *addr, socklen_t *addrlen)
{
	proto_unix_conn_t *conn = net_proto_unix_conn_find (fd);

	if (!conn)
		return -1;

	unsigned i = 0;

	proto_unix_backlog_t *backlog = 0;

	/* blocking mode */
	if (!(conn->flags & O_NONBLOCK)) {
		while (!i) {
			for (backlog = proto_unix_backlog_list.next; backlog != &proto_unix_backlog_list; backlog = backlog->next) {
				if (backlog->conn == conn) {
					i = 1;
					break;
				}
			}
	
			Schedule ();
		}
	} else {
	/* non-blocking mode */
		for (backlog = proto_unix_backlog_list.next; backlog != &proto_unix_backlog_list; backlog = backlog->next) {
			if (backlog->conn == conn) {
				i = 1;
				break;
			}
		}
	}

	if (!i)
		return -1;

	fd_t *fd_new = fd_create (FD_SOCK);
	
	if (!fd_new)
		return -1;
	
	net_proto_unix_conn_add (fd_new);

	proto_unix_conn_t *conn_new = net_proto_unix_conn_find (fd_new->id);

	if (!conn_new)
		return -1;

	backlog->session->session = conn_new;
	backlog->session->state = PROTO_UNIX_CONN_STATE_ESTABILISHED;

	net_proto_unix_conn_set (conn_new, conn->path, 0);

	conn_new->session = (void *) backlog->session;

	/* remove from queue accepted connection */
	backlog->next->prev = backlog->prev;
	backlog->prev->next = backlog->next;

	DEL (backlog);

	return fd_new->id;
}

/** UNIX protocol
 *  hardcore code :P
 */

int net_proto_unix_write (proto_unix_conn_t *conn, char *data, unsigned len)
{
	if (!data || !conn || !len)
		return 0;

	conn = (proto_unix_conn_t *) conn->session;

	if (!conn)
		return 0;

	if (!conn->len)
		conn->data = (char *) NEW (sizeof (char) * (len + 1));
	else
		conn->data = (char *) krealloc (conn->data, (sizeof (char) * (conn->len+len)));

	if (!conn->data)
		return -1;

	memcpy (conn->data+conn->len, data, len);

	conn->len += len;

	return conn->len;
}


/* Create new UNIX connection */
int net_proto_unix_conn_add (fd_t *fd)
{
	proto_unix_conn_t *conn;

	/* alloc and init context */
	conn = (proto_unix_conn_t *) NEW (sizeof (proto_unix_conn_t));

	if (!conn) {
		errno_set (ENOMEM);
		return -1;
	}

	memset (conn, 0, sizeof (proto_unix_conn_t));

	conn->flags = 0;
	conn->path = 0;
	conn->session = 0;

	conn->fd = fd->id;

	/* add into list */
	conn->next = &proto_unix_conn_list;
	conn->prev = proto_unix_conn_list.prev;
	conn->prev->next = conn;
	conn->next->prev = conn;

	return 0;
}

/* Setup new connection */
int net_proto_unix_conn_set (proto_unix_conn_t *conn, char *path, unsigned char bind)
{
	if (!conn)
		return 0;

	unsigned l = strlen (path);

	if (!l)
		return 0;

	conn->path = (char *) NEW (sizeof (char) * (l + 1)); 

	if (!conn->path)
		return 0;

	memcpy (conn->path, path, l);
	conn->path[l] = '\0';

	conn->len = 0;
	conn->data = 0;
	conn->bind = bind;

	return 1;
}

/* Delete existing connection from list */
unsigned net_proto_unix_conn_del (proto_unix_conn_t *conn)
{
	if (!conn)
		return 0;

	if (conn->len)
		DEL (conn->data);

	if (conn->path)
		DEL (conn->path);

	conn->len = 0;

	conn->next->prev = conn->prev;
	conn->prev->next = conn->next;

	DEL (conn);

	return 1;
}

proto_unix_conn_t *net_proto_unix_conn_find (int fd)
{
	proto_unix_conn_t *conn = NULL;
	for (conn = proto_unix_conn_list.next; conn != &proto_unix_conn_list; conn = conn->next) {
		if (conn->fd == fd)
			return conn;
	}
	
	return 0;
}

proto_unix_conn_t *net_proto_unix_conn_findbypath (char *path)
{
	proto_unix_conn_t *conn = NULL;
	for (conn = proto_unix_conn_list.next; conn != &proto_unix_conn_list; conn = conn->next) {
		if (!conn->path)
			continue;

		if (!strcmp (conn->path, path) && conn->bind)
			return conn;
	}
	
	return 0;
}

/* Create new UNIX backlog stamp */
int net_proto_unix_backlog_add (proto_unix_conn_t *conn, proto_unix_conn_t *session)
{
	if (!conn)
		return 0;

	proto_unix_backlog_t *backlog;

	/* alloc and init context */
	backlog = (proto_unix_backlog_t *) NEW (sizeof (proto_unix_backlog_t));

	if (!backlog)
		return 0;

	backlog->conn = conn;
	backlog->session = session;

	/* add into list */
	backlog->next = &proto_unix_backlog_list;
	backlog->prev = proto_unix_backlog_list.prev;
	backlog->prev->next = backlog;
	backlog->next->prev = backlog;
	
	return 1;
}

/* init of unix protocol */
unsigned init_net_proto_unix ()
{
	proto_unix_conn_list.next = &proto_unix_conn_list;
	proto_unix_conn_list.prev = &proto_unix_conn_list;

	proto_unix_backlog_list.next = &proto_unix_backlog_list;
	proto_unix_backlog_list.prev = &proto_unix_backlog_list;

	return 1;
}
