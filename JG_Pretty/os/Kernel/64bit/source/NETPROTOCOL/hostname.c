
#include "hostname.h"
#include "../DynamicMemory.h"

/* global hostname */
static char *net_hostname;

unsigned hostname_set (const char *name)
{
	if (net_hostname)
		DEL (net_hostname);

	unsigned l = strlen (name);
	
	net_hostname = (char *) NEW (sizeof (char) * (l + 1));
	
	if (!net_hostname)
		return 0;
	
	memcpy (net_hostname, name, l);
	net_hostname[l] = '\0';
	
	return 1;
}

char *hostname_get ()
{
	if (!net_hostname)
		return 0;
	
	return net_hostname;
}

unsigned init_hostname ()
{
	net_hostname = 0;
  
	return hostname_set ("CONFIG_HOSTNAME");
}
