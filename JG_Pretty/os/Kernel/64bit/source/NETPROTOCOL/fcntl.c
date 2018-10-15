/*
 *  ZeX/OS
 *  Copyright (C) 2007  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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


#include "socket.h"
#include "file.h"
#include "fd.h"
#include "../DynamicMemory.h"
#include "../Utility.h"

int fcntl (int fd, int cmd, long arg)
{
	fd_t *fd_p = fd_get (fd);

	if (!fd_p)
		return -1;

	if (fd_p->flags & FD_SOCK)	// socket fcntl
		return sfcntl (fd, cmd, arg);

	switch (cmd) {
		case F_GETFL:
			return fd_p->flags;
		case F_SETFL:
			fd_p->flags = arg;
			return 0;
		case F_DUPFD:
/*			if (!arg)
				close ((int) arg);

			fd_t *fd = fd_create (fd_p->flags);
			
			if (!fd)
				return -1;
		
			fd->e = fd_p->e;
			fd->s = fd_p->s;
			fd->p = fd_p->p;
			
			unsigned fi_len = strlen (fd_p->path);
			fd->path = NEW (fi_len + 1);
			
			if (!fd->path)
				return -1;
			
			memcpy (fd->path, fd_p->path, fi_len);
			fd->path[fi_len] = '\0';
		
			return fd->id;*/
			return -1;
	}

	return -1;
}
