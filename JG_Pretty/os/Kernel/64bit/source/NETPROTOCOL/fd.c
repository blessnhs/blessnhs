/*
 *  ZeX/OS
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2009  Martin 'povik' Poviser (martin.povik@gmail.com)
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

#include "fd.h"
#include "../errno.h"
#include "../Utility.h"
#include "../DynamicMemory.h"

fd_t fd_list;

fd_t *fd_get (int id)
{
	fd_t *fd;
	for (fd = fd_list.next; fd != &fd_list; fd = fd->next) {
		if (fd->id == (unsigned) id)
			return fd;
	}

	return 0;
}

fd_t *fd_create (int flags)
{
	int id = 0;
	fd_t *fd = NEW (sizeof (fd_t));

	if (!fd) {
		errno_set (ENOMEM);
		return 0;
	}
	
	while (id >= 0) {
		id ++;
		if (!fd_get (id))
			break;
	}
	
	if (id < 0) {
		errno_set (ENFILE);
		return 0;
	}
	
	fd->id = id;
	fd->flags = flags;

	fd->e = 1;
	fd->s = (char *) 0;
	fd->p = 0;
	fd->path = 0;
//	fd->proc = 0;

	/* add into list */
	fd->next = &fd_list;
	fd->prev = fd_list.prev;
	fd->prev->next = fd;
	fd->next->prev = fd;

	return fd;
}

int fd_delete (fd_t *fd)
{
	if (!fd)
		return 0;

	if (fd->path)
		DEL (fd->path);
	
	fd->s = 0;
	fd->e = 0;
	fd->flags = 0;
	fd->id = 0;
	fd->p = 0;
//	fd->dev = 0;
//	fd->proc = 0;
	
	/* delete old file descriptor from fd_list */
	fd->next->prev = fd->prev;
	fd->prev->next = fd->next;

	DEL (fd);

	return 1;
}

unsigned int init_fd ()
{
	fd_list.next = &fd_list;
	fd_list.prev = &fd_list;

	/* HACK: first we need create file descriptor 0 and 1 for stdout and stdin */
	stdin = fd_create (0);
	stdout = fd_create (0);

	return 1;
}
