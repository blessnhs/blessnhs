/*
 *  ZeX/OS
 *  Copyright (C) 2007  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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


#include "socket.h"
#include "../task.h"
#include "fd.h"
#include "../errno.h"
#include "../DynamicMemory.h"

socket_t socket_list;

socket_t *socket_getbyfd (int fd)
{
	socket_t *socket;
	for (socket = socket_list.next; socket != &socket_list; socket = socket->next) {
		if (socket->fd == fd)
			return socket;
	}

	return 0;
}

int socket (int family, int type, int protocol)
{
	/* alloc and init context */
	socket_t *sock = (socket_t *) NEW (sizeof (socket_t));

	if (!sock) {
		errno_set (ENOMEM);
		return -1;
	}

	sock->family = family;
	sock->type = type;
	sock->protocol = protocol;


	fd_t *fd = fd_create (FD_SOCK);
	if (!fd) {
		DEL (sock);
		return -1;
	}

	sock->fd = fd->id;
	
	int ret = 0;

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			ret = net_proto_unix_socket (fd);
			break;
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					ret = net_proto_tcp_socket (fd);
					break;
				case IPPROTO_UDP:
					ret = net_proto_udp_socket (fd);
					break;
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					ret = net_proto_tcp6_socket (fd);
					break;
				case IPPROTO_UDP:
					ret = net_proto_udp6_socket (fd);
					break;
			}
			break;
		case AF_RS232:
			ret = ips_socket (fd);
			break;
	}
	
	if (ret) {
		DEL (sock);
		fd_delete (fd);
		
		Printf("socket () -> %d : ERROR", sock->fd);
		return -1;
	}

	/* add into list */
	sock->next = &socket_list;
	sock->prev = socket_list.prev;
	sock->prev->next = sock;
	sock->next->prev = sock;

/*	proc_t *proc = proc_find (_curr_task);

//	 when it is process
	if (proc) {
		if (proc->task != _curr_task)
			return;

		fd->proc = proc;
	} else {
		// it is kernel socket
		fd->proc = 0;
	}
*/
//	Printf("socket () -> socket: %d", sock->fd);

	return sock->fd;
}

hostent *gethostbyname (char *hostname)
{
	unsigned i = 0;
	unsigned g = 0;
	unsigned l = strlen (hostname);

	/* TODO: make better checking, that it is IPv6 addresss */
	while (i < l) {
		if (hostname[i] == ':')
			g ++;

		i ++;
	}

	unsigned target = 0;
	unsigned short targetv6[8];

	/* Is it an IPv6 address */
	if (g == 7) {
		if (!net_proto_ipv6_convert (targetv6, hostname))
			return 0;
	} else {
		i = 0;
		g = 0;

		/* TODO: make better checking, that it is IPv4 addresss */
		while (i < l) {
			if (hostname[i] == '.')
				g ++;

			i ++;
		}

		/* it is dns address */
		if (g != 3) {

			if (dns_cache_get (hostname, &target, 4) != 1)			/* check for ipv4 address */
				if (dns_send_request (hostname, &target, 4) != 1) {
					g = 7;
					if (dns_cache_get (hostname, &targetv6, 16) != 1) {		/* check for ipv6 address */
						if (dns_send_request (hostname, &targetv6, 16) != 1)
							return 0;
					}
			}
		} else
			target = net_proto_ip_convert (hostname);

		if (!target)
			return 0;
	}

	hostent *host = (hostent *) NEW (sizeof (hostent));

	if (!host)
		return 0;

	memset (host, 0, sizeof (hostent));

	/* IPv4 or IPv6 ? */
	if (g == 7)
		host->h_length = 16;	/* IPv6 - 16 bytes */
	else
		host->h_length = 4;	/* IPv4 - 4 bytes */

	host->h_addr = (char *) NEW (sizeof (char) * host->h_length);

	if (!host->h_addr) {
		DEL (host);
		return 0;
	}

	if (g == 7)
		memcpy (host->h_addr, &targetv6, host->h_length);
	else
		memcpy (host->h_addr, &target, host->h_length);
	return host;
}

int connect (int fd, sockaddr *addr, socklen_t len)
{

//	Printf("connect () -> socket: %d", fd);
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_connect (fd, addr);
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_connect (fd, (sockaddr_in *) addr);
				case IPPROTO_UDP:
					return net_proto_udp_connect (fd, addr);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_connect (fd, (sockaddr_in *) addr);
				case IPPROTO_UDP:
					return net_proto_udp6_connect (fd, (sockaddr_in6 *) addr);
			}
			break;
		case AF_RS232:
			return ips_connect (fd, addr);
	}
		
	return 0;
}

int send (int fd, char *msg, size_t size, int flags)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
	{
		Printf("send socket_getbyfd %d fail\n", fd);
		return -1;
	}

//	Printf("send (%d) -> %s", fd, msg);

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_send (fd, msg, size);
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_send (fd, msg, size);
				case IPPROTO_UDP:
					return net_proto_udp_send (fd, msg, size);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_send (fd, msg, size);
				case IPPROTO_UDP:
					return net_proto_udp6_send (fd, msg, size);
			}
			break;
//		case AF_RS232:
//			return ips_send (fd, msg, size);
	}

	return 0;
}

int sendto (int fd, const void *msg, size_t len, int flags, sockaddr *to, socklen_t tolen)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("sendto (%d) -> %s", fd, msg);

	switch (sock->protocol) {
		case IPPROTO_TCP:
			switch (sock->family) {
				case AF_UNSPEC:
					return -1;
				case AF_UNIX:
					return -1;
				case AF_INET:
					return -1; // net_proto_tcp_sendto (fd, msg, len, to);
				case AF_INET6:
					return -1; // net_proto_tcp6_sendto (fd, msg, len, to);
				case AF_RS232:
					return -1;
			}
			break;
		case IPPROTO_UDP:
			switch (sock->family) {
				case AF_UNSPEC:
					return -1;
				case AF_UNIX:
					return -1;
				case AF_INET:
					return net_proto_udp_sendto (fd, msg, len, (sockaddr_in *) to);
				case AF_INET6:
					return net_proto_udp6_sendto (fd, msg, len, (sockaddr_in6 *) to);
				case AF_RS232:
					return -1;
			}
			break;
	}

	return 0;
}

int recv (int fd, char *msg, size_t size, int flags)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("recv (%d)", fd);

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_recv (fd, msg, size);
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_recv (fd, msg, size);
				case IPPROTO_UDP:
					return net_proto_udp_recv (fd, msg, size);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_recv (fd, msg, size);
				case IPPROTO_UDP:
					return net_proto_udp6_recv (fd, msg, size);
			}
			break;
		case AF_RS232:
			return ips_recv (fd, msg, size);
	}

	return -1;
}

int recvfrom (int fd, char *msg, size_t size, int flags, sockaddr *from, socklen_t fromlen)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("recvfrom (%d)", fd);

	switch (sock->protocol) {
		case IPPROTO_TCP:
			switch (sock->family) {
				case AF_UNSPEC:
					return -1;
				case AF_UNIX:
					return -1;
				case AF_INET:
					return -1; //net_proto_tcp_recvfrom (fd, msg, size, from);
				case AF_INET6:
					return -1; //net_proto_tcp6_recvfrom (fd, msg, size, from);
				case AF_RS232:
					return -1;
			}
			break;
		case IPPROTO_UDP:
			switch (sock->family) {
				case AF_UNSPEC:
					return -1;
				case AF_UNIX:
					return -1;
				case AF_INET:
					return net_proto_udp_recvfrom (fd, msg, size, (sockaddr_in *) from);
				case AF_INET6:
					return net_proto_udp6_recvfrom (fd, msg, size, (sockaddr_in6 *) from);
				case AF_RS232:
					return -1;
			}
			break;
	}

	return -1;
}

int bind (int fd, sockaddr *addr, socklen_t len)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("bind (%d)", fd);

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_bind (fd, (sockaddr_in *) addr, len);
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_bind (fd, (sockaddr_in *) addr);
				case IPPROTO_UDP:
					return net_proto_udp_bind (fd, (sockaddr_in *) addr, len);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_bind (fd, (sockaddr_in6 *) addr, len);
				case IPPROTO_UDP:
					return net_proto_udp6_bind (fd, (sockaddr_in6 *) addr, len);
			}
			break;
		case AF_RS232:
			return ips_bind (fd, addr);
	}

	return -1;
}

int listen (int fd, int backlog)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("listen (%d)", fd);

	switch (sock->family) {
		case AF_UNSPEC:
			
			return -1;
		case AF_UNIX:
			return net_proto_unix_listen (fd, backlog);
		case AF_INET:
			return net_proto_tcp_listen (fd, backlog);
		case AF_INET6:
			return net_proto_tcp6_listen (fd, backlog);
		case AF_RS232:
			return ips_listen (fd, backlog);
	}

	return -1;
}

int accept (int fd, sockaddr *addr, socklen_t *addrlen)
{
	socket_t *servsock = socket_getbyfd (fd);

	if (!servsock)
		return -1;

	//DPRINT (DBG_NET | DBG_SOCKET, "accept (%d)", fd);

	int client = 0;

	switch (servsock->family) {
		case AF_UNSPEC:
			
			return -1;
		case AF_UNIX:
			client = net_proto_unix_accept (fd, (sockaddr_in *) addr, addrlen);
			break;
		case AF_INET:
			client = net_proto_tcp_accept (fd, (sockaddr_in *) addr, addrlen);
			break;
		case AF_INET6:
			client = net_proto_tcp6_accept (fd, (sockaddr_in6 *) addr, addrlen);
			break;
		case AF_RS232:
			client = ips_accept (fd, addr, addrlen);
			break;
	}

	if (client <= 0)
		return client;

	socket_t *sock;

	/* alloc and init context */
	sock = (socket_t *) NEW (sizeof (socket_t));

	if (!sock)
		return 0;
	
	sock->fd = client;
	sock->family = servsock->family;
	sock->type = servsock->type;
	sock->protocol = servsock->protocol;

	/* add into list */
	sock->next = &socket_list;
	sock->prev = socket_list.prev;
	sock->prev->next = sock;
	sock->next->prev = sock;

	return client;
}

int sclose (int fd)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("close (%d)", fd);

	fd_t *pfd = fd_get (fd);
	if (!pfd)
	{
		DEL (pfd);
		DEL (sock);
	}

	/* delete old socket from socket_list */
	sock->next->prev = sock->prev;
	sock->prev->next = sock->next;

	int family = sock->family;
	int protocol = sock->protocol;

	DEL (sock);
	switch (family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_close (fd);
		case AF_INET:
			switch (protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_close (fd);
				case IPPROTO_UDP:
					return net_proto_udp_close (fd);
			}
			break;
		case AF_INET6:
			switch (protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_close (fd);
				case IPPROTO_UDP:
					return net_proto_udp6_close (fd);
			}
			break;
		case AF_RS232:
			return ips_close (fd);
	}

	return -1;
}

int sfcntl (int fd, int cmd, long arg)
{
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;

//	Printf("fcntl (%d)", fd);

	switch (sock->family) {
		case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_fcntl (fd, cmd, arg);
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_fcntl (fd, cmd, arg);
				case IPPROTO_UDP:
					return net_proto_udp_fcntl (fd, cmd, arg);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_fcntl (fd, cmd, arg);
				case IPPROTO_UDP:
					return net_proto_udp6_fcntl (fd, cmd, arg);
			}
			break;
		case AF_RS232:
			return ips_fcntl (fd);
	}
  
	return -1;
}

/* socket api select for specified socket */
int sselect (int readfd, int writefd, int exceptfd)
{
	int fd = -1;
  
	if (readfd)
		fd = readfd;
	else if (writefd)
		fd = writefd;
	else if (exceptfd)
		fd = exceptfd;
	
	socket_t *sock = socket_getbyfd (fd);

	if (!sock)
		return -1;
		
	switch (sock->family) {
		/*case AF_UNSPEC:
			return -1;
		case AF_UNIX:
			return net_proto_unix_select (readfd, writefd, exceptfd);*/
		case AF_INET:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp_select (readfd, writefd, exceptfd);
				case IPPROTO_UDP:
					return net_proto_udp_select (readfd, writefd, exceptfd);
			}
			break;
		case AF_INET6:
			switch (sock->protocol) {
				case IPPROTO_TCP:
					return net_proto_tcp6_select (readfd, writefd, exceptfd);
				case IPPROTO_UDP:
					return net_proto_udp6_select (readfd, writefd, exceptfd);
			}
			break;
		/*case AF_RS232:
			return ips_select (readfd, writefd, exceptfd);*/
	}
  
	return -1;
}

unsigned int init_socket ()
{
	socket_list.next = &socket_list;
	socket_list.prev = &socket_list;

	return 1;
}

