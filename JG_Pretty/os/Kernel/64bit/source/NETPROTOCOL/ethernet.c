/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f�r die Verwendung dieses Sourcecodes siehe unten
*/

#include "ethernet.h"
#include "arp.h"
#include "ipv4.h"
#include "ipv6.h"
#include "../DynamicMemory.h"
#include "../types.h"

extern Packet_t lastPacket; // network.c

static const uint8_t broadcast_MAC_FF[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t broadcast_MAC_00[6] = {0, 0, 0, 0, 0, 0};


void ethernet_received(network_adapter_t* adapter, const ethernet_t* eth, uint32_t length)
{
  #ifdef _NETWORK_DATA_
    Printf("\n\n>> Packet received. <<");
    Printf("\nEthernet: ");
  #endif
    if (MemCmp(eth->recv_mac, adapter->MAC, 6) != 0 && MemCmp(eth->recv_mac, broadcast_MAC_FF, 6) != 0 && MemCmp(eth->recv_mac, broadcast_MAC_00, 6) != 0)
    {
      #ifdef _NETWORK_DATA_
        Printf("We are not the addressee.");
      #endif
        return;
    }

  #ifdef _NETWORK_DATA_
    Printf("\nLength: %u", length);

    Printf(" %M\t<== %M\n", eth->recv_mac, eth->send_mac); // MAC adresses

    if (eth->type_len <= 1500)
        Printf("Type 1, Length: ");
    else
        Printf("Type 2, Type: ");

    Printf("%xh ", eth->type_len);
  #endif

    MemCpy(lastPacket.MAC, eth->send_mac, 6); // save sender

    if (ntohs(eth->type_len) > 1500) // Ethernet 2
    {
        // cf. http://en.wikipedia.org/wiki/EtherType
        // and http://www.cavebear.com/archive/cavebear/Ethernet/type.html

        // now we look for IPv4, IPv6, or ARP
        switch (ntohs(eth->type_len))
        {
            case 0x0800: // IPv4
                ipv4_received(adapter, (const void*)(eth+1), length-sizeof(ethernet_t));
                break;
            case 0x0806: // ARP
                arp_received(adapter, (const void*)(eth+1));
                break;
            case 0x86DD: // IPv6
                ipv6_received(adapter, (const void*)(eth+1), length - sizeof(ethernet_t));
                break;
            default:
              #ifdef _NETWORK_DATA_
                Printf("Protocol type: %xh - Neither IP nor ARP.", eth->type_len);
              #endif
                break;
        }
    }
    else // Ethernet 1
    {
      #ifdef _NETWORK_DATA_
        Printf("Ethernet 1. ");
      #endif
    }
}

bool ethernet_send(network_adapter_t* adapter, const void* data, uint32_t length, const uint8_t MAC[6], uint16_t type, uint8_t offloading)
{
  #ifdef _NETWORK_DATA_
    Printf("\nEth send:");
    Printf(" %M ==> %M", adapter->MAC, MAC);
    Printf("  length = %u.", sizeof(ethernet_t) + length);
  #endif
    if (sizeof(ethernet_t) + length > 0x700)
    {
      #ifdef _NETWORK_DATA_
        Printf("\nError: Packet is longer than 0x700 (1792) Bytes\n");
      #endif
        return false;
    }

    ethernet_t* packet = AllocateMemory(sizeof(ethernet_t)+length);

    MemCpy(packet+1, data, length);
    MemCpy(packet->recv_mac, MAC, 6);
    MemCpy(packet->send_mac, adapter->MAC, 6);
    packet->type_len = htons(type);

    bool retVal = network_sendPacket(adapter, (void*)packet, length + sizeof(ethernet_t), offloading);
    FreeMemory(packet);
    return 1;
}

/*
* Copyright (c) 2009-2016 The PrettyOS Project. All rights reserved.
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
