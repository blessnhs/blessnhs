#ifndef UDP_H
#define UDP_H

#include "../network/network.h"
#include "../task.h"
#include "../network/events.h"

typedef struct
{
    IP4_t     srcIP;
    uint16_t srcPort;
    uint16_t destPort;
    size_t   length;
} __attribute__((packed)) udpReceivedEventHeader_t;

typedef struct
{
    uint16_t sourcePort;
    uint16_t destPort;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udpPacket_t;


typedef struct
{
	event_queue_t   eventQueue;
    uint16_t port;
    bool isActivate;
} udp_port_t;

bool udp_bind(uint16_t port);
void udp_unbind(uint16_t port);
void udp_receive(network_adapter_t* adapter, const udpPacket_t* packet, IP4_t sourceIP);
void udp_send(network_adapter_t* adapter, const void* data, uint32_t length, uint16_t srcPort, IP4_t srcIP, uint16_t destPort, IP4_t destIP);
bool udp_usend(const void* data, uint32_t length, IP4_t destIP, uint16_t srcPort, uint16_t destPort);
void udp_cleanup(uint16_t port);
udp_port_t* udp_findport(uint16_t port);

#endif
