
#ifndef _FCNTL_H
#define	_FCNTL_H

/* file modes */
#define O_RDWR		0x1
#define O_RDONLY	0x2
#define O_WRONLY	0x4
#define O_CREAT		0x200
#define O_TRUNC		0x400
#define O_NONBLOCK 	0x800
#define O_EXCL		0x1000

/* externs */
extern int fcntl (int fd, int cmd, long arg);

#endif 
