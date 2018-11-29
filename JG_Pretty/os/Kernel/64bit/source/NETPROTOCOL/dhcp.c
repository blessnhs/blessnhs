

#include "if.h"
#include "ip.h"
#include "dhcp.h"
#include "socket.h"
#include "hostname.h"
#include "../DynamicMemory.h"


#define DHCP_REQUEST_MAX_LEN 4096

int dhcp_request_header_prepare (dhcp_request_t request, netif_t *netif, 
				 char req_type)
{
	request.bootp_header->op = req_type; /* Request type */
	request.bootp_header->htype = 0x01; /* Connection type : Ethernet */ /* TODO */
	request.bootp_header->hlen = 0x06; /* Hardware address length : 6 bytes */
	request.bootp_header->hops = 0x00; /* Clients set to zero */
	request.bootp_header->xid = 0x12345678; /* Random request ID */
	request.bootp_header->secs = 0x0000; /* Is'nt needed in dhcp request */
	request.bootp_header->flags = 0x0000; /* No flags */
	
	request.bootp_header->ciaddr = NET_IPV4_TO_ADDR (0, 0, 0, 0);
	request.bootp_header->yiaddr = NET_IPV4_TO_ADDR (0, 0, 0, 0);
	request.bootp_header->siaddr = NET_IPV4_TO_ADDR (0, 0, 0, 0);
	request.bootp_header->giaddr = NET_IPV4_TO_ADDR (0, 0, 0, 0);
	
	memcpy (request.bootp_header->chaddr, netif->dev->dev_addr, 6);
	
	request.bootp_header->sname[0] = 0;
	request.bootp_header->file[0] = 0;
	
	request.bootp_header->mag_cookie = DHCP_MAGIC_COOKIE;
	
	return sizeof (dhcp_bootp_header_t);
}

int dhcp_request_option_add (dhcp_request_t request, char option_type, char *buffer,
			     int buffer_len, int pos)
{
	request.buffer[pos] = option_type;
	request.buffer[pos + 1] = buffer_len;
	memcpy (request.buffer + pos + 2, buffer, buffer_len);

	return pos + 2 + buffer_len;
}

int dhcp_config_if (netif_t *netif)
{
	net_ipv4 server_addr = INADDR_BROADCAST;
	sockaddr_in server_socket;
	int sock = 0;
	
	unsigned xid;
	unsigned int pos;
	dhcp_request_t request;
	
	if (!netif)
	{
		Printf("dhcp_config_if is null \n" );
		return DHCP_BAD_PARAMETERS;
	}
	
	request.buffer = NEW (DHCP_REQUEST_MAX_LEN);

	if (!request.buffer)
		return DHCP_OUT_OF_MEMORY;
	  
	char *buffer = NEW (1024);

	if (!buffer) {
		DEL (request.buffer);
		return DHCP_OUT_OF_MEMORY;
	}
	
	net_ipv4 config_gw = 0x00000000;
	net_ipv4 config_ip = 0x00000000;
	net_ipv4 config_dns = 0x00000000;
	net_ipv4 config_mask = 0x00000000;
	net_ipv4 config_dhcp = 0x00000000;
	unsigned int config_lease_time = 0;
	
	int ret = 0;
	
	/* Create a socket */
	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		Printf("DHCP client -> Can't create socket.");
		ret = DHCP_SOCKET_FAILED;
		goto clear;
	}
	
	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons (67);
	memcpy (&(server_socket.sin_addr), &server_addr, sizeof (net_ipv4));
	
	/* Let's connect to server */
	if (connect (sock, (sockaddr *) &server_socket,
	    sizeof (server_socket)) == -1) {
		Printf("DHCP client -> Connection can't be estabilished");
		ret = DHCP_CONNECT_FAILED;
		goto clear;
	}
	
	net_proto_udp_anycast (sock);
	net_proto_udp_port (sock, htons (68));
	
	pos = dhcp_request_header_prepare (request, netif,
						DHCP_MESSAGE_TYPE_BOOTREQUEST);
	xid = request.bootp_header->xid;
		
	buffer[0] = DHCP_OPTION_VALUE_DHCPDISCOVER;
	pos = dhcp_request_option_add (request, DHCP_OPTION_MESSAGE_TYPE,
						buffer, 1, pos);
	
	buffer[0] = 0x01;
	memcpy (buffer + 1, netif->dev->dev_addr, 6);
	pos = dhcp_request_option_add (request, DHCP_OPTION_CLIENT_IDENTIFIER,
						buffer, 7, pos);
						
	char *hostname = hostname_get ();
	
	if (strlen (hostname) <= 128) {
		unsigned host_len = strlen (hostname);
		pos = dhcp_request_option_add (request, DHCP_OPTION_HOSTNAME,
					       buffer, host_len, pos);
	}
	
	buffer[0] = DHCP_OPTION_VALUE_SUBNET_MASK;
	buffer[1] = DHCP_OPTION_VALUE_ROUTER;
	buffer[2] = DHCP_OPTION_VALUE_DNS_SERVER;
	buffer[3] = DHCP_OPTION_VALUE_DOMAIN_NAME;
	pos = dhcp_request_option_add (request, DHCP_OPTION_PARAMETER_REQUEST_LIST,
					buffer, 0x04, pos);
	
	int recv_ret;
	
	Printf("DHCP client -> sending request\n");
	if (send (sock, request.buffer, pos, 0) < 0) {
		Printf("DHCP client -> Can't send request.");
		ret = DHCP_CANT_SEND_REQUEST;
		goto clear;
	}
	Printf("DHCP client -> reuquest sended\n");
	
recv_dhcpoffer:
	config_ip = 0x00000000;
	config_gw = 0x00000000;
	config_dns = 0x00000000;
	config_dhcp = 0x00000000;
	
	Printf ("DHCP client -> receiving response\n");
	if ((recv_ret = recv (sock, request.buffer, DHCP_REQUEST_MAX_LEN, 0)) < sizeof (dhcp_bootp_header_t)) {
		Printf("DHCP client -> Can't receive response. %d\n",recv_ret);
		ret = DHCP_CANT_RECV_RESPONSE;
		goto clear;
	}
	
	Printf("DHCP client -> response received %s\n",request.buffer);

	if (request.bootp_header->mag_cookie != DHCP_MAGIC_COOKIE) {
		Printf("DHCP client -> bad magic (finded 0x%X - needed 0x%X",
			request.bootp_header->mag_cookie, DHCP_MAGIC_COOKIE);
		ret = DHCP_BAD_PACKET;
		goto clear;
	}
	
	pos = sizeof (dhcp_bootp_header_t);
	
	config_ip = request.bootp_header->yiaddr;

	if (request.bootp_header->xid != xid) {
		Printf("DHCP client -> bad xid");
		goto recv_dhcpoffer;
	}
	
	char dhcpack = 0;
	while (pos < recv_ret) {
		unsigned int option_len;

		if (request.buffer[pos] == DHCP_OPTION_PAD) {
			pos += 1;
			continue;
		}

		if (request.buffer[pos] == DHCP_OPTION_END)
			break;

		if (pos >= recv_ret - 2) {
			ret = DHCP_BAD_PACKET;
			goto clear;
		}

		option_len = request.buffer[pos + 1];
		pos += 2;

		if (pos + option_len >= recv_ret) {
			ret = DHCP_BAD_PACKET;
			goto clear;
		}
		switch (request.buffer[pos - 2]) {
			case DHCP_OPTION_MESSAGE_TYPE:
				if (!option_len == 1) {
					ret = DHCP_BAD_PACKET;
					goto clear;
				}
				switch (request.buffer[pos]) {
					case DHCP_OPTION_VALUE_DHCPOFFER:
						break;
					case DHCP_OPTION_VALUE_DHCPACK:
						dhcpack = 1;
						break;
					case DHCP_OPTION_VALUE_DHCPNAK:
						ret = DHCP_SRV_DIDNT_UNDERSTAND;
						goto clear;
						break;
					default:
						Printf("DHCP client -> bad DHCP message type");
						goto recv_dhcpoffer;
				}
				break;
			case DHCP_OPTION_SUBNET_MASK:
				if (option_len >= 4)
					memcpy (&config_mask, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_ROUTER:
				if (option_len >= 4)
					memcpy (&config_gw, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_DNS_SERVER:
				if (option_len >= 4)
					memcpy (&config_dns, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_DHCP_SERVER:
				if (option_len >= 4)
					memcpy (&config_dhcp, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_LEASE_TIME:
				if (option_len >= 4)
					memcpy (&config_lease_time, request.buffer + pos, sizeof (unsigned int));
				break;
			default:
				break;
		}
		
		pos += option_len;
	}
	
	Printf("DHCP client -> response parsed\n");

	if (dhcpack)
		goto configure;

	pos = dhcp_request_header_prepare (request, netif,
						DHCP_MESSAGE_TYPE_BOOTREQUEST);
	
	buffer[0] = DHCP_OPTION_VALUE_DHCPREQUEST;
	pos = dhcp_request_option_add (request, DHCP_OPTION_MESSAGE_TYPE,
						buffer, 1, pos);

	memcpy (buffer, &config_ip, 4);
	pos = dhcp_request_option_add (request, DHCP_OPTION_REQUESTED_IP,
						buffer, 4, pos);
	
	memcpy (buffer, &config_dhcp, 4);
	pos = dhcp_request_option_add (request, DHCP_OPTION_DHCP_SERVER,
						buffer, 4, pos);
	
	Printf("DHCP client -> sending request\n");
	if (send (sock, request.buffer, pos, 0) < 0) {
		Printf("DHCP client -> Can't send request.\n");
		ret = DHCP_CANT_SEND_REQUEST;
		goto clear;
	}
	Printf("DHCP client -> request sended\n");
	
recv_dhcpack:

Printf("DHCP client -> receiving response\n");
	if ((recv_ret = recv (sock, request.buffer, DHCP_REQUEST_MAX_LEN, 0)) < sizeof (dhcp_bootp_header_t)) {
		Printf("DHCP client -> Can't receive response. %d",recv_ret);
		ret = DHCP_CANT_RECV_RESPONSE;
		goto clear;
	}
	Printf("DHCP client -> response received\n");
	
	if (request.bootp_header->mag_cookie != DHCP_MAGIC_COOKIE) {
		Printf("DHCP client -> bad magic (finded 0x%X - needed 0x%X",
			request.bootp_header->mag_cookie, DHCP_MAGIC_COOKIE);
		ret = DHCP_BAD_PACKET;
		goto clear;
	}

	if (request.bootp_header->xid != xid) {
		Printf("DHCP client -> bad xid");
		goto recv_dhcpoffer;
	}
	
	pos = sizeof (dhcp_bootp_header_t);
	
	while (pos < recv_ret) {
		unsigned int option_len;
		if (request.buffer[pos] == DHCP_OPTION_PAD) {
			pos += 1;
			continue;
		}

		if (request.buffer[pos] == DHCP_OPTION_END)
			break;
	
		if (pos >= recv_ret - 2) {
			ret = DHCP_BAD_PACKET;
			goto clear;
		}
		
		option_len = request.buffer[pos + 1];
		pos += 2;
		
		if (pos + option_len >= recv_ret) {
			ret = DHCP_BAD_PACKET;
			goto clear;
		}
		
		switch (request.buffer[pos - 2]) {
			case DHCP_OPTION_MESSAGE_TYPE:
				if (!option_len == 1) {
					ret = DHCP_BAD_PACKET;
					goto clear;
				}
				switch (request.buffer[pos]) {
					case DHCP_OPTION_VALUE_DHCPOFFER:
						ret = DHCP_BAD_PACKET;
						goto clear;
						break;
					case DHCP_OPTION_VALUE_DHCPACK:
						break;
					case DHCP_OPTION_VALUE_DHCPNAK:
						ret = DHCP_SRV_DIDNT_UNDERSTAND;
						goto clear;
						break;
					default:
						Printf("DHCP client -> bad DHCP message type");
						goto recv_dhcpoffer;
				}
				break;
			case DHCP_OPTION_SUBNET_MASK:
				if (option_len >= 4)
					memcpy (&config_mask, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_ROUTER:
				if (option_len >= 4)
					memcpy (&config_gw, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_DNS_SERVER:
				if (option_len >= 4)
					memcpy (&config_dns, request.buffer + pos, sizeof (net_ipv4));
				break;
			case DHCP_OPTION_LEASE_TIME:
				if (option_len >= 4)
					memcpy (&config_lease_time, request.buffer + pos, sizeof (unsigned int));
				break;
			default:
				break;
		}
	
		pos += option_len;
	}
	
configure:
	Printf("DHCP client -> configuring interface\n");

	char *ip_str = NEW (17); // 17 = IPv4 address in string maximum size + 1 zero

	if (ip_str) {
		memset (ip_str, 0, 17);
		net_proto_ip_convert2 (config_ip, ip_str);
		Printf("DHCP client -> my IP: %s\n", ip_str);
		
		memset (ip_str, 0, 17);
		net_proto_ip_convert2 (config_gw, ip_str);
		Printf("DHCP client -> router IP: %s\n", ip_str);
		
		memset (ip_str, 0, 17);
		net_proto_ip_convert2 (config_dns, ip_str);
		Printf("DHCP client -> DNS server IP: %s\n", ip_str);
		
		Printf("DHCP client -> IP lease time: %d seconds\n", config_lease_time);
		DEL (ip_str);
	}	


	//2018.10.15 nhs
	//�ش� �ּҷ� �����ϸ� dns arp���� �����̾��� ������ �� �κ��� �ּ� ó����

//	dns_addr (config_dns);
	netif_ip_addr (netif, config_ip, IF_CFG_TYPE_DHCP);
	netif_gw_addr (netif, config_gw);
clear:
	DEL (buffer);
	DEL (request.buffer);
	sclose (sock);
	
	return ret;
}

