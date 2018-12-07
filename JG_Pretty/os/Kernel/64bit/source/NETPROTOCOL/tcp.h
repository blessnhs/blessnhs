
#ifndef _TCP_H
#define _TCP_H

#include "ip.h"
#include "socket.h"
#include "../types.h"

// IP protocol version 4
#define	IP_PROTOCOL_VERSION_4		4

// fragment flags/offset mask
#define IP_DONT_FRAGMENT			0x4000	/* dont fragment flag */
#define IP_FRAGMENT_OFFSET_MASK		0x1fff	/* mask for fragment offset */

// Internet implementation parameters.
#define IP_MAX_TIME_TO_LIVE			255		/* maximum time to live */
#define IP_DEFAULT_TIME_TO_LIVE		64		/* default ttl, from RFC 1340 */
#define ETH_ALEN			6

#define ETHERTYPE_IP		0x0800	// IP
#define ETHERTYPE_ARP		0x0806	// Address resolution

#define ETHER_MIN_TRANSFER_UNIT	46
#define ETHER_MAX_TRANSFER_UNIT	1500

// IP protocols
#define IPPROTO_TCP					6
#define IPPROTO_UDP					17

#define TRACE Printf
#define TRACE_QUEUE Printf
#define TRACE_CHECKSUM Printf
#define TRACE_PORT Printf

#define PROTO_TCP_CONN_STATE_ESTABILISH		0x1
#define PROTO_TCP_CONN_STATE_ESTABILISHED	0x2
#define PROTO_TCP_CONN_STATE_DISCONNECTED	0x4
#define PROTO_TCP_CONN_STATE_WAIT		0x8
#define PROTO_TCP_CONN_STATE_READY		0x10
#define PROTO_TCP_CONN_STATE_CLOSE		0x20
#define PROTO_TCP_CONN_STATE_ESTABILISHERROR	0x40

enum TCPSocketState {
	TCP_SOCKET_STATE_INITIAL,
	TCP_SOCKET_STATE_SYN_SENT,
	TCP_SOCKET_STATE_SYN_RECEIVED,
	TCP_SOCKET_STATE_OPEN,
	TCP_SOCKET_STATE_FIN_SENT,
	TCP_SOCKET_STATE_CLOSED
};

/* TCP connection structure */
typedef struct proto_tcp_conn_context {
	struct proto_tcp_conn_context *next, *prev;

	net_ipv4 ip_source;
	net_ipv4 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned checkedask;

	unsigned seq;
	unsigned ack;

	unsigned short flags;

	unsigned char state;

	unsigned char bind;

	unsigned short fd;

	unsigned char cross;

	netif_t *netif;

	unsigned int len;


	uint32_t		fNextSequence;
	struct TCPPacket*	fFirstPacket;
	struct TCPPacket*	fLastPacket;
	struct TCPPacket*	fFirstSentPacket;
	struct TCPPacket*	fLastSentPacket;
	enum TCPSocketState fState;
	enum TCPSocketState fRemoteState;

	long close_wait_time;

} proto_tcp_conn_t;

struct pseudo_header {
	net_ipv4	source;
	net_ipv4	destination;
	uint8_t		pad;
	uint8_t		protocol;
	unsigned short		length;
} __attribute__ ((__packed__));


/* TCPv6 connection structure */
typedef struct proto_tcp6_conn_context {
	struct proto_tcp6_conn_context *next, *prev;

	net_ipv6 ip_source;
	net_ipv6 ip_dest;

	net_port port_source;
	net_port port_dest;

	unsigned seq;
	unsigned ack;

	unsigned short flags;

	unsigned char state;

	unsigned char bind;

	unsigned short fd;

	unsigned char cross;

	netif_t *netif;

	unsigned short offset;
	unsigned short len;
	char *data;

} proto_tcp6_conn_t;

/* TCP server backlog structure */
typedef struct proto_tcp_backlog_context {
	struct proto_tcp_backlog_context *next, *prev;

	net_ipv4 ip;

	net_port port;

	unsigned seq;

	proto_tcp_conn_t *conn;
} proto_tcp_backlog_t;

/* TCPv6 server backlog structure */
typedef struct proto_tcp6_backlog_context {
	struct proto_tcp6_backlog_context *next, *prev;

	net_ipv6 ip;

	net_port port;

	unsigned seq;

	proto_tcp6_conn_t *conn;
} proto_tcp6_backlog_t;

/* TCP layer structure */
typedef struct proto_tcp_t {
	net_port port_source:16;		// 2
	net_port port_dest:16;		// 4

	unsigned seq:32;			// 8
	unsigned ack:32;			// 12

	unsigned char res:4;		// 13
	unsigned char data_offset:4;	

	unsigned char flags:8;		// 14

	unsigned short window:16;		// 16

	unsigned short checksum:16;	// 18
	unsigned short urgentPointer:16;
}  __attribute__((packed)) proto_tcp_t;


#define TCP_FIN		(1 << 0)
#define TCP_SYN		(1 << 1)
#define TCP_RST		(1 << 2)
#define TCP_PSH		(1 << 3)
#define TCP_ACK		(1 << 4)
#define TCP_URG		(1 << 5)
#define TCP_ECE		(1 << 6)	// RFC 3168
#define TCP_CWR		(1 << 7)	// RFC 3168

enum
{
	CHAIN_BUFFER_HEAD			= 0x1,
	CHAIN_BUFFER_EMBEDDED_DATA	= 0x2,
	CHAIN_BUFFER_FREE_DATA		= 0x4,
	CHAIN_BUFFER_ON_STACK		= 0x8,
};

struct ChainBuffer {
	uint32_t		fFlags:4;
	uint32_t		fSize:14;
	uint32_t		fTotalSize:14;
	void			*fData;
	struct ChainBuffer	*fNext;
	uint8_t		fBuffer[1];
};

typedef struct TCPPacket
{
	net_ipv4	fSourceAddress;
	net_ipv4	fDestinationAddress;
	uint16_t	fSourcePort;
	uint16_t	fDestinationPort;
	uint32_t	fSequenceNumber;
	uint32_t	fAcknowledgmentNumber;
	void*		fData;
	int			fSize;
	uint8_t		fFlags;
	struct TCPPacket*		fNext;
};

struct Iterator
{
	struct ChainBuffer	*fBuffer;
	int			fOffset;
};


proto_tcp_conn_t *net_proto_tcp_conn_check (net_ipv4 ip_source, net_port port_source, net_ipv4 ip_dest, net_port port_dest, unsigned char *ret);
void Iterator(struct Iterator *iter,struct ChainBuffer *buffer);

bool HasNext(struct Iterator *iter);

uint16_t Next(struct Iterator *iter);

void _Next(struct Iterator *iter);

uint8_t _NextByte(struct Iterator *iter);
uint16_t ip_checksum(struct ChainBuffer *buffer);
uint16_t _Checksum(struct proto_ip_t *header);
void Flatten(struct ChainBuffer *this,void *_buffer);

unsigned init_net_proto_tcp ();
unsigned init_net_proto_tcp6 ();

unsigned int _rand32(void);
unsigned short _rand14(void);

int accept (int fd, sockaddr *addr, socklen_t *addrlen);
int net_proto_tcp_connect (int fd, sockaddr_in *addr);
void HandleIPPacket(net_ipv4 sourceIP,net_ipv4 destinationIP, const void* data, size_t size);
int _WaitForState(struct proto_tcp_conn_context *conn,enum TCPSocketState state, long timeout);

int SetTo(struct TCPPacket *packet,const void* data, int size, net_ipv4 sourceAddress,
	uint16_t sourcePort, net_ipv4 destinationAddress, uint16_t destinationPort,
	uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint8_t flags);

int Write(struct proto_tcp_conn_context *context,const void* buffer, int bufferSize);

unsigned short _ChecksumBuffer(struct ChainBuffer* buffer, net_ipv4 source,net_ipv4 destination, unsigned short length);
int SSend(struct proto_tcp_conn_context *context,struct TCPPacket* packet, bool enqueue);
int ___Send(net_ipv4 destination, uint8_t protocol, struct ChainBuffer *buffer);
int ESend(mac_addr_t destination, uint16_t protocol,struct ChainBuffer *buffer);
void ChainBuffer(struct ChainBuffer *this,void *data, uint32_t size, struct ChainBuffer *next,bool freeData);
void Init(struct ChainBuffer *this,void *data, uint32_t size, struct ChainBuffer *next, uint32_t flags);
void Append(struct ChainBuffer *buffer,struct ChainBuffer *next);
struct ChainBuffer *DetachNext(struct ChainBuffer *buffer);
#endif
