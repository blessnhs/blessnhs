
#ifndef _DHCP_H
#define _DHCP_H

#include "if.h"
#include "ip.h"

/* error codes */
#define DHCP_BAD_PARAMETERS			-1
#define DHCP_OUT_OF_MEMORY			-2
#define DHCP_SOCKET_FAILED			-3
#define DHCP_CONNECT_FAILED			-4
#define DHCP_CANT_SEND_REQUEST			-5
#define DHCP_CANT_RECV_RESPONSE			-6
#define DHCP_BAD_PACKET				-7
#define DHCP_SRV_DIDNT_UNDERSTAND		-8

/* magic cookie */
#define DHCP_MAGIC_COOKIE 			0x63538263

/* message types */
#define DHCP_MESSAGE_TYPE_BOOTREQUEST		0x01
#define DHCP_MESSAGE_TYPE_BOOTREPLY		0x02

/* options types + values*/
#define DHCP_OPTION_PAD				0x00
#define DHCP_OPTION_SUBNET_MASK			0x01
#define DHCP_OPTION_ROUTER			0x03
#define DHCP_OPTION_DNS_SERVER			0x06
#define DHCP_OPTION_HOSTNAME			12
#define DHCP_OPTION_REQUESTED_IP		50
#define DHCP_OPTION_LEASE_TIME			51
#define DHCP_OPTION_MESSAGE_TYPE		53
#define 	DHCP_OPTION_VALUE_DHCPDISCOVER	0x01
#define  	DHCP_OPTION_VALUE_DHCPOFFER	0x02
#define  	DHCP_OPTION_VALUE_DHCPREQUEST	0x03
#define  	DHCP_OPTION_VALUE_DHCPDECLINE	0x04
#define  	DHCP_OPTION_VALUE_DHCPACK	0x05
#define  	DHCP_OPTION_VALUE_DHCPNAK	0x06
#define  	DHCP_OPTION_VALUE_DHCPRELEASE	0x07
#define  	DHCP_OPTION_VALUE_DHCPINFORM	0x08
#define DHCP_OPTION_DHCP_SERVER			54
#define DHCP_OPTION_PARAMETER_REQUEST_LIST	55
#define 	DHCP_OPTION_VALUE_SUBNET_MASK	0x01
#define 	DHCP_OPTION_VALUE_ROUTER	0x03
#define 	DHCP_OPTION_VALUE_DNS_SERVER	0x06
#define 	DHCP_OPTION_VALUE_DOMAIN_NAME	0x0f
#define DHCP_OPTION_CLIENT_IDENTIFIER		61
#define DHCP_OPTION_END				0xff

typedef struct dhcp_bootp_header_t {
	char op;
	char htype;
	char hlen;
	char hops;
	unsigned xid;
	unsigned short secs;
	unsigned short flags;
	net_ipv4 ciaddr;
	net_ipv4 yiaddr;
	net_ipv4 siaddr;
	net_ipv4 giaddr;
	char chaddr[16];
	char sname[64];
	char file[128];
	unsigned int mag_cookie;
} dhcp_bootp_header_t;

typedef union dhcp_request_t {
	unsigned char *buffer;
	dhcp_bootp_header_t *bootp_header;
} dhcp_request_t;

extern int dhcp_config_if (netif_t *netif);

#endif
