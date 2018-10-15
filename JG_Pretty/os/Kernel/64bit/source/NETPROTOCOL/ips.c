/*
 *  ZeX/OS
 *  Copyright (C) 2007  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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
#include "../Synchronization.h"
#include "../types.h"

#define IPS_PROTO_CONNECT 	"/#ips#c"
#define IPS_PROTO_CONNECTED 	"/#ips#k"
#define IPS_PROTO_RECV 		"/#ips#r"
#define IPS_PROTO_EOF 		"/#ips#e"
#define IPS_PROTO_CLOSE		"/#ips#q"
#define IPS_PROTO_SOCKET	"/#ips#s"
#define IPS_PROTO_BIND		"/#ips#b"
#define IPS_PROTO_LISTEN	"/#ips#l"
#define IPS_PROTO_ACCEPT	"/#ips#a"
#define IPS_PROTO_FCNTL		"/#ips#f"

/* Mutex for socket function */
MUTEX_CREATE (mutex_ips);

/** Internet Protocol over Serial - IPS **
 *  For easy connection to network
 */

int ips_socket ()
{
	int len = 0;
	int fd = 0;

/*	char cmd[] = IPS_PROTO_SOCKET;
	ips_send2 (fd, cmd, 7);

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	fd = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

//	Printf ("ips_socket (): %d\n", fd);

	Unlock (&mutex_ips);
*/
	return fd;
}

int ips_connect (int fd, sockaddr_in *addr)
{
	int len = 0;
	/*
	char data[40];
	char ip[32];

	if (!net_proto_ip_convert2 (addr->sin_addr, ip))
		return 0;

	sPrintf (data, IPS_PROTO_CONNECT " %s %d", ip, addr->sin_port);

	ips_send2 (fd, data, strlen (data));

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	len = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	Unlock (&mutex_ips);
*/
	return len;
}

int ips_send22 (int fd, char *msg, size_t size)
{
	int len = 0;
	/*
	Lock (&mutex_ips);

	rs232_write ((unsigned char) size);
	rs232_write ((unsigned char) (size >> 8));
	rs232_write ((unsigned char) (size >> 16));
	rs232_write ((unsigned char) (size >> 24));

	rs232_write ((unsigned char) fd);
	rs232_write ((unsigned char) (fd >> 8));
	rs232_write ((unsigned char) (fd >> 16));
	rs232_write ((unsigned char) (fd >> 24));

	while (len != size) {
		rs232_write (msg[len]);
		len ++;
	}

//	Printf ("send (%d): %d, '%s', %d\n", len, fd, msg, size);

	Unlock (&mutex_ips);
*/
	return len;
}

int ips_recv (int fd, char *msg, size_t size)
{
	int len = 0;
	/*
	char data[40];

	sPrintf (data, IPS_PROTO_RECV " %d", size);
	ips_send2 (fd, data, strlen (data));

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	len = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	if (len < 1) {
		Unlock (&mutex_ips);
		return len;
	}

	if (len > size) {
		Unlock (&mutex_ips);
		return -2;
	}

	int l = 0;
	while (l != len) {
		msg[l] = rs232_read ();
		l ++;
	}

	Unlock (&mutex_ips);
*/
	return len;
}

int ips_bind (int fd, sockaddr_in *addr)
{
	int ret = 0;
	/*
	char data[40];

	ips_send2 (fd, data, strlen (data));

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	ret = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	Unlock (&mutex_ips);
*/
	return ret;
}

int ips_listen (int fd, int backlog)
{
	int ret = 0;

/*	char data[40];

	sPrintf (data, IPS_PROTO_LISTEN " %d", backlog);
	ips_send2 (fd, data, strlen (data));

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	ret = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	Unlock (&mutex_ips);
*/
	return ret;
}

int ips_accept (int fd, sockaddr_in *addr, socklen_t *addrlen)
{
	int ret = 0;
/*
	char cmd[] = IPS_PROTO_ACCEPT;
	ips_send2 (fd, cmd, 7);

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	ret = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	Unlock (&mutex_ips);
*/
	return ret;
}

int ips_close (int fd)
{
//	Lock (&mutex_ips);

//	char cmd[] = IPS_PROTO_CLOSE;
//	ips_send2 (fd, cmd, 7);
	
//	Printf ("ips_close (%d)\n", fd);

//	Unlock (&mutex_ips);

	return 1;
}

int ips_fcntl (int fd, int cmd, long arg)
{
	int ret = 0;

	char data[40];
/*
	sPrintf (data, IPS_PROTO_FCNTL " %d %ld", cmd, arg);
	ips_send2 (fd, data, strlen (data));

	Lock (&mutex_ips);

	char num[5];
	num[0] = rs232_read ();
	num[1] = rs232_read ();
	num[2] = rs232_read ();
	num[3] = rs232_read ();

	ret = num[0] | (num[1] << 8) | (num[2] << 16) | (num[3] << 24);

	Unlock (&mutex_ips);
*/
	return ret;
}

