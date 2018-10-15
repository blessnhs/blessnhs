

#include "Errno.h"

static int *errno;	/* shared errno variable */

void errno_set (int error)
{
	*errno = error;
}

int errno_get ()
{
	return *errno;
}

unsigned int init_errno ()
{
	errno = (int *) ERRNO_ADDRESS;

	return 1;
}
