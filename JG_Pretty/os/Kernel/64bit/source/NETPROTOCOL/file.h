/*
 *  ZeX/OS
 *  Copyright (C) 2007  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
 *  Copyright (C) 2008  Tomas 'ZeXx86' Jedrzejek (zexx86@zexos.org)
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


#ifndef _FILE_H
#define _FILE_H

#define O_RDWR		0x1
#define O_RDONLY	0x2
#define O_WRONLY	0x4
#define O_CREAT		0x200
#define O_TRUNC		0x400
#define O_NONBLOCK 	0x800

#define F_DUPFD		0x0	/* dup */
#define F_GETFD		0x1	/* get close_on_exec */
#define F_GETFL 	0x3
#define F_SETFL 	0x4

typedef struct {
	char *ptr;
	unsigned long e;
	unsigned mode;
	unsigned fd;
} FILE;

#endif 
