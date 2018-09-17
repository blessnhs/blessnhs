/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f�r die Verwendung dieses Sourcecodes siehe unten
*/

// http://www.rfc-editor.org/rfc/rfc768.txt <---  User Datagram Protocol (UDP)

#include "udp.h"
#include "ipv4.h"
#include "../task.h"
#include "../DynamicMemory.h"
#include "../Utility.h"
#include "../network/events.h"

static list_t udpPorts = list_init();


#ifdef _UDP_DEBUG_
static void udp_debug(const udpPacket_t* udp);
#endif


static udp_port_t* findConnection(uint16_t port)
{
	element* e;
    for(e = udpPorts.head; e != 0; e = e->next)
    {
        udp_port_t* connection = e->data;
        if(connection->port == port)
            return (connection);
    }
    return (0);
}

bool udp_bind(uint16_t port)
{
    udp_port_t* udpPort = findConnection(port);
    if(udpPort)
    {
    	Printf("Cant Found udp_bind port %d\n",port);
    	return (false);
    }

    element* elem = list_alloc_elem(sizeof(udp_port_t), "udp_port_t");
    udpPort = elem->data;
    udpPort->port = port;
    udpPort->isActivate = false;
    event_createQueue(&udpPort->eventQueue);
    list_append_elem(&udpPorts, elem);
    return (true);
}

void udp_unbind(uint16_t port)
{
	element* e;
    for(e = udpPorts.head; e != 0; e = e->next)
    {
        udp_port_t* udpPort = e->data;
        if(udpPort->port == port)
        {
            list_delete(&udpPorts, e);
            return;
        }
    }
}

udp_port_t* udp_findport(uint16_t port)
{
	element* e;
	for(e = udpPorts.head; e != 0; e = e->next)
	{
	   udp_port_t* connection = e->data;
	   if(connection->port == port)
	      return (connection);
	}
	return (0);
}

void udp_cleanup(uint16_t port)
{
	element* e;
    for(e = udpPorts.head; e != 0;)
    {
        udp_port_t* udpPort = e->data;
        if(udpPort->port == port)
        {
            e = list_delete(&udpPorts, e);
        }
        else
            e = e->next;
    }
}

void udp_receive(network_adapter_t* adapter, const udpPacket_t* packet, IP4_t sourceIP)
{
  #ifdef _UDP_DEBUG_
    Printf("\nUDP: ");
    udp_debug(packet);
  #endif

    switch (ntohs(packet->destPort))
    {
        case 68:
            DHCP_AnalyzeServerMessage(adapter, (const dhcp_t*)(packet+1), sourceIP);
            break;
        default:
        {
          #ifdef _UDP_DEBUG_
            Printf("\nUDP arbitrary port");
          #endif

            udpReceivedEventHeader_t* ev = AllocateMemory(sizeof(udpReceivedEventHeader_t) + ntohs(packet->length));
            MemCpy(ev+1, packet+1, ntohs(packet->length));
            ev->length   = ntohs(packet->length);
            ev->srcIP    = sourceIP;
            ev->srcPort  = ntohs(packet->sourcePort);
            ev->destPort = ntohs(packet->destPort);

            udp_port_t* udpPort = findConnection(ntohs(packet->destPort));
            if(udpPort == 0)
            {
            	 Printf("\n udp_receive find port fail \n");
            	 FreeMemory(ev);
            	 return;
            }


          	event_issue(&udpPort->eventQueue, EVENT_UDP_RECEIVED, ev, sizeof(udpReceivedEventHeader_t) + ntohs(packet->length));

            FreeMemory(ev);
            break;
        }
    }
}

void udp_send(network_adapter_t* adapter, const void* data, uint32_t length, uint16_t srcPort, IP4_t srcIP, uint16_t destPort, IP4_t destIP)
{
    udpPacket_t* packet = AllocateMemory(sizeof(udpPacket_t)+length);
    MemCpy(packet+1, data, length);

    packet->sourcePort  = htons(srcPort);
    packet->destPort    = htons(destPort);
    packet->length      = htons(length + sizeof(udpPacket_t));
    packet->checksum    = 0; // 0 necessary for successful DHCP Process
    // packet->checksum = 0; // for checksum calculation
    //packet->checksum = htons(udptcpCalculateChecksum((void*)packet, length + sizeof(udpPacket_t), srcIP, destIP, 17));

    ipv4_send(adapter, packet, length + sizeof(udpPacket_t), destIP, 17, adapter->features & OFFLOAD_UDP);
    FreeMemory(packet);
}


// user programs
bool udp_usend(const void* data, uint32_t length, IP4_t destIP, uint16_t srcPort, uint16_t destPort)
{
    network_adapter_t* adapter = network_getFirstAdapter();

    if (adapter)
    {
        udp_send(adapter, data, length, srcPort, adapter->IP, destPort, destIP);
        return true;
    }
    return false;
}

#ifdef _UDP_DEBUG_
static void printUDPPortType(uint16_t port)
{
    // http://www.iana.org/assignments/port-numbers
    // http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers

    switch (port)
    {
    case 20:
        Printf("FTP - data transfer");
        break;
    case 21:
        Printf("FTP - control (command)");
        break;
    case 22:
        Printf("Secure Shell (SSH)");
        break;
    case 53:
        Printf("Domain Name System (DNS)");
        break;
    case 67:
        Printf("DHCPv4 Server");
        break;
    case 68:
        Printf("DHCPv4 Client");
        break;
    case 80:
        Printf("HTTP");
        break;
    case 137:
        Printf("NetBIOS Name Service");
        break;
    case 138:
        Printf("NetBIOS Datagram Service");
        break;
    case 139:
        Printf("NetBIOS Session Service");
        break;
    case 143:
        Printf("IMAP)");
        break;
    case 546:
        Printf("DHCPv6 Client");
        break;
    case 547:
        Printf("DHCPv6 Server");
        break;
    case 1257:
        Printf("shockwave2");
        break;
    case 1900:
        Printf("SSDP");
        break;
    case 3544:
        Printf("Teredo Tunneling");
        break;
    case 5355:
        Printf("LLMNR)");
        break;
    default:
        Printf("Port: %u", port);
        break;
    }

}

static void udp_debug(const udpPacket_t* udp)
{
    Printf("  %u ==> %u   Len: %u\n", ntohs(udp->sourcePort), ntohs(udp->destPort), ntohs(udp->length));
    printUDPPortType(ntohs(udp->sourcePort)); Printf(" ==> "); printUDPPortType(ntohs(udp->destPort));
}
#endif


/*
* Copyright (c) 2010-2016 The PrettyOS Project. All rights reserved.
*
* http://www.prettyos.de
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
