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


#ifndef _FD_H
#define _FD_H

/* File descriptor structure */
typedef struct fd_context {
	struct fd_context *next, *prev;

	char *s;
	unsigned long e;
	unsigned flags;
	unsigned id;
	unsigned p;
	char *path;
} fd_t;

/* input/output posix file descriptors */
fd_t *stdout;
fd_t *stdin;

/* externs */
extern fd_t fd_list;

extern fd_t *fd_get (int id);
extern fd_t *fd_create (int flags);
extern int fd_delete (fd_t *fd);
extern unsigned int init_fd ();

#endif 
