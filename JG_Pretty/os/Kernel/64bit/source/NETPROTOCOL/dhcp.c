/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f�r die Verwendung dieses Sourcecodes siehe unten
*/

#include "dhcp.h"
#include "udp.h"

static uint32_t xid = 0x00E0FF0A; // AFFE.. Transaction Code for Identification

// Sets up basic data of a dhcp packet.
static void DHCP_preparePacket(dhcp_t* packet, network_adapter_t* adapter)
{
    packet->op         = 1;
    packet->htype      = 1;         // type: for ethernet and 802.11 wireless clients, the hardware type is always 01
    packet->hlen       = 6;
    packet->hops       = 0;
    packet->xid        = xid;       // AFFExx
    packet->secs       = htons(0);  // must use the same value in Discover and Request
    packet->flags      = BROADCAST;
    packet->ciaddr.iIP4 = 0;
    packet->yiaddr.iIP4 = 0;
    packet->siaddr.iIP4 = 0;
    packet->giaddr.iIP4 = 0;

    MemCpy(packet->chaddr, adapter->MAC, 6);
    MemSet(packet->chaddr + 6, 0, 10);
    MemSet(packet->sname, 0, 64);
    MemSet(packet->file, 0, 128);

    // options
    packet->options[0]  =  99;  // MAGIC
    packet->options[1]  = 130;  // MAGIC
    packet->options[2]  =  83;  // MAGIC
    packet->options[3]  =  99;  // MAGIC

    MemSet(packet->options + 4, 255, OPTIONSIZE - 4); // fill with end token
}

extern Packet_t lastPacket; // network.c    // TODO: should be eliminated on a long run

void DHCP_Discover(network_adapter_t* adapter)
{
    xid += (1<<24);

  #ifdef _DHCP_DEBUG_
    textColor(IMPORTANT);
    Printf("\nDHCP Discover sent.");
    textColor(TEXT);
  #endif

    dhcp_t packet;
    DHCP_preparePacket(&packet, adapter);

    packet.options[4]  =  53;  // MESSAGE TYPE  // must option
    packet.options[5]  =   1;  // Length
    packet.options[6]  =   1;  // DISCOVER

    packet.options[7]  =  57;  // MAX MESSAGE SIZE
    packet.options[8]  =   2;  // Length
    packet.options[9]  =   2;  // (data) 2*256 //
    packet.options[10] =  64;  // (data)    64 // max size: 576

    packet.options[11] =  50;  // Requested IP
    packet.options[12] =   4;  // Length
    packet.options[13] = RIP_1;
    packet.options[14] = RIP_2;
    packet.options[15] = RIP_3;
    packet.options[16] = RIP_4;

    packet.options[17]  = 55;  // Parameter request list
    packet.options[18]  =  4;  // Length
    packet.options[19]  =  1;  // SUBNET MASK
    packet.options[20] =   3;  // ROUTERS
    packet.options[21] =   6;  // DOMAIN NAME SERVER
    packet.options[22] =  15;  // DOMAIN NAME

    packet.options[23] =  12;  // Hostname
    packet.options[24] =   8;  // Length
    packet.options[25] = 'P';  // P
    packet.options[26] = 'r';  // r
    packet.options[27] = 'e';  // e
    packet.options[28] = 't';  // t
    packet.options[29] = 't';  // t
    packet.options[30] = 'y';  // y
    packet.options[31] = 'O';  // O
    packet.options[32] = 'S';  // S

    IP4_t srcIP = {.iIP4 = 0};
    IP4_t destIP = {.iIP4 = 0xFFFFFFFF};
    udp_send(adapter, &packet, sizeof(dhcp_t), 68, srcIP, 67, destIP);
}

void DHCP_Request(network_adapter_t* adapter, IP4_t requestedIP)
{
    xid += (1<<24);

  #ifdef _DHCP_DEBUG_
    textColor(IMPORTANT);
    Printf("\nDHCP Request sent.");
    textColor(TEXT);
  #endif

    dhcp_t packet;
    DHCP_preparePacket(&packet, adapter);

    packet.options[4]  = 53;  // MESSAGE TYPE  // must option
    packet.options[5]  =  1;  // Length
    packet.options[6]  =  3;  // REQUEST

    packet.options[7]  = 55;  // Parameter request list
    packet.options[8]  =  4;  // Length
    packet.options[9]  =  1;  // SUBNET MASK
    packet.options[10] =  3;  // ROUTERS
    packet.options[11] =  6;  // DOMAIN NAME SERVER
    packet.options[12] = 15;  // DOMAIN NAME

    packet.options[13] = 54;  // Server IP
    packet.options[14] =  4;  // Length
    packet.options[15] =  adapter->Gateway_IP.IP4[0]; /// TEST /////////////////////////////////////////////// TEST ///
    packet.options[16] =  adapter->Gateway_IP.IP4[1];
    packet.options[17] =  adapter->Gateway_IP.IP4[2];
    packet.options[18] =  adapter->Gateway_IP.IP4[3];

    packet.options[19] = 61;  // Client Identifier - hardware type and client hardware address
    packet.options[20] =  7;  // Length
    packet.options[21] =  1;  // Type: for ethernet and 802.11 wireless clients, the hardware type is always 01
    uint8_t i = 0;
    for (i = 0; i < 6; i++)
        packet.options[22+i] = adapter->MAC[i];

    packet.options[28] =  50;  // Requested IP
    packet.options[29] =   4;  // Length
    packet.options[30] = requestedIP.IP4[0];
    packet.options[31] = requestedIP.IP4[1];
    packet.options[32] = requestedIP.IP4[2];
    packet.options[33] = requestedIP.IP4[3];

    packet.options[34] =  12;  // Hostname
    packet.options[35] =   8;  // Length
    packet.options[36] = 'P';  // P
    packet.options[37] = 'r';  // r
    packet.options[38] = 'e';  // e
    packet.options[39] = 't';  // t
    packet.options[40] = 't';  // t
    packet.options[41] = 'y';  // y
    packet.options[42] = 'O';  // O
    packet.options[43] = 'S';  // S

    IP4_t srcIP  = {.iIP4 = 0x00000000};
    IP4_t destIP = {.iIP4 = 0xFFFFFFFF};

    udp_send(adapter, &packet, sizeof(dhcp_t), 68, srcIP, 67, destIP);
}

void DHCP_Inform(network_adapter_t* adapter)
{
    xid += (1<<24);

  #ifdef _DHCP_DEBUG_
    textColor(IMPORTANT);
    Printf("\nDHCP Inform sent.");
    textColor(TEXT);
  #endif

    dhcp_t packet;
    DHCP_preparePacket(&packet, adapter);

    packet.ciaddr.iIP4 = adapter->IP.iIP4;

    packet.options[4]  =   1;  // SUBNET
    packet.options[5]  =   4;  // Length
    packet.options[6]  = 255;
    packet.options[7]  = 255;
    packet.options[8]  = 255;
    packet.options[9]  =   0;

    packet.options[10]  = 53;  // MESSAGE TYPE
    packet.options[11]  =  1;  // Length
    packet.options[12]  =  8;  // INFORM

    packet.options[13]  = 55;  // Parameter Request List
    packet.options[14]  =  8;  // Length
    packet.options[15]  =  1;  // SUBNET MASK
    packet.options[16] =   3;  // ROUTERS
    packet.options[17] =   6;  // DOMAIN NAME SERVER
    packet.options[18] =  15;  // DOMAIN NAME
    packet.options[19] =  28;  // BROADCAST ADDRESS
    packet.options[20] =  31;  // Perform Router Discover
    packet.options[21] =  33;  // Static Route
    packet.options[22] =  42;  // Network Time Protocol (NTP) SERVERS

    packet.options[23] =  61;  // Client Identifier - hardware type and client hardware address
    packet.options[24] =   7;  // Length
    packet.options[25] =   1;  // Type: for ethernet and 802.11 wireless clients, the hardware type is always 01

    uint8_t i = 0;
    for (i = 0; i < 6; i++)
        packet.options[26+i] = adapter->MAC[i];

    IP4_t destIP = {.iIP4 = 0xFFFFFFFF};
    udp_send(adapter, &packet, sizeof(dhcp_t), 68, adapter->IP, 67, destIP);
}

void DHCP_Release(network_adapter_t* adapter)
{
    xid += (1<<24);

  #ifdef _DHCP_DEBUG_
    textColor(IMPORTANT);
    Printf("\nDHCP Release sent.");
    textColor(TEXT);
  #endif

    dhcp_t packet;
    DHCP_preparePacket(&packet, adapter);

    packet.flags = UNICAST;
    packet.ciaddr.iIP4 = adapter->IP.iIP4;

    packet.options[4]  = 53;  // MESSAGE TYPE
    packet.options[5]  =  1;  // Length
    packet.options[6]  =  7;  // RELEASE

    packet.options[7]  = 61;  // Client Identifier - hardware type and client hardware address
    packet.options[8]  =  7;  // Length
    packet.options[9]  =  1;  // Type: for ethernet and 802.11 wireless clients, the hardware type is always 01
    uint8_t i = 0;
    for (i = 0; i < 6; i++)
        packet.options[10+i] = adapter->MAC[i];

    IP4_t destIP = {.iIP4 = 0xFFFFFFFF};

    udp_send(adapter, &packet, sizeof(dhcp_t), 68, adapter->IP, 67, destIP);
}

static void DHCP_AnalyzeOptions(network_adapter_t* adapter, const uint8_t* opt);

void DHCP_AnalyzeServerMessage(network_adapter_t* adapter, const dhcp_t* dhcp, IP4_t sourceIP)
{
  #ifdef _DHCP_DEBUG_
    textColor(HEADLINE);
    Printf("\nDHCP:");

    textColor(LIGHT_GRAY); Printf("\ncIP: "); textColor(IMPORTANT); Printf("%I4", dhcp->ciaddr);
    textColor(LIGHT_GRAY); Printf("\tyIP: "); textColor(IMPORTANT); Printf("%I4", dhcp->yiaddr);
    textColor(LIGHT_GRAY); Printf("\tsIP: "); textColor(IMPORTANT); Printf("%I4", dhcp->siaddr);
    textColor(LIGHT_GRAY); Printf("\tgIP: "); textColor(IMPORTANT); Printf("%I4", dhcp->giaddr);
    textColor(LIGHT_GRAY); Printf("\nMAC: "); textColor(IMPORTANT); Printf("%M\n", dhcp->chaddr);
  #endif
    DHCP_AnalyzeOptions(adapter, dhcp->options);
    switch (adapter->DHCP_State)
    {
        case DHCP_OFFER:
          #ifdef _DHCP_DEBUG_
            Printf("\n >>> PrettyOS got a DHCP OFFER. <<<\n");
          #endif
            if (dhcp->yiaddr.iIP4 != 0)
            {
                arp_addTableEntry(&adapter->arpTable, lastPacket.MAC, sourceIP, false);
                DHCP_Request(adapter, dhcp->yiaddr);
            }
            else
            {
                adapter->IP.IP4[0] = IP_1;
                adapter->IP.IP4[1] = IP_2;
                adapter->IP.IP4[2] = IP_3;
                adapter->IP.IP4[3] = IP_4;
            }
            break;
        case DHCP_ACK:
            Printf("\n >>> PrettyOS got a DHCP ACK. <<<\n");
            if (dhcp->yiaddr.iIP4 != 0)
            {
                adapter->IP.iIP4 = dhcp->yiaddr.iIP4;
            }
          #ifdef _DHCP_DEBUG_
            Printf("\nGateway IP: %I4 Subnet: %I4", adapter->Gateway_IP, adapter->Subnet);
          #endif
            break;
        case DHCP_NAK:
            Printf("\n >>> DHCP was not successful (NAK). <<<\n");
            break;
        default:
            break;
    }
}

static uint16_t showOptionsBytes(network_adapter_t* adapter, const uint8_t* opt, uint16_t count)
{
  #ifdef _NETWORK_DATA_
    uint32_t leaseTime=0;
  #endif
	uint16_t i=0;

    switch (opt[count+1]) // 1: message  2: length  3 to (2+length): data
    {
        case 1: // subnet mask
          #ifdef _NETWORK_DATA_
            for (i=0; i<opt[count+2]; i++)
            {
                Printf("%u ", opt[count+3+i]);
            }
          #endif

            if (opt[count+2] == 4)
            {
                MemCpy(adapter->Subnet.IP4, opt+count+3, 4);
            }
            break;

      #ifdef _NETWORK_DATA_
        case 12: case 14: case 15: case 17: case 18: case 40: case 43: // ASCII output
            for (i=0; i<opt[count+2]; i++)
                Printf("%c", opt[count+3+i]);
            break;
      #endif
        case 51: // Lease time
          #ifdef _NETWORK_DATA_
            for (i=0; i<opt[count+2]; i++)
            {
                Printf("%u ", opt[count+3+i]);
                leaseTime += ((opt[count+3+i])<<(24-8*i));
            }
            Printf("  %u hours", leaseTime/3600);
          #endif
            break;
        case 53: // Message type
          #ifdef _NETWORK_DATA_
            for (i=0; i<opt[count+2]; i++)
                Printf("%u ", opt[count+3+i]);
          #endif
            switch (opt[count+3])
            {
              #ifdef _NETWORK_DATA_
                case 1:
                    Printf(" (DHCPDiscover)");
                    break;
                case 2:
                    Printf(" (DHCPOffer)");
                    adapter->DHCP_State = DHCP_OFFER;
                    break;
                case 3:
                    Printf(" (DHCPRequest)");
                    break;
                case 4:
                    Printf(" (DHCPDecline)");
                    break;
                case 5:
                    Printf(" (DHCPAck)");
                    adapter->DHCP_State = DHCP_ACK;
                    break;
                case 6:
                    Printf(" (DHCPNak)");
                    adapter->DHCP_State = DHCP_NAK;
                    break;
                case 7:
                    Printf(" (DHCPRelease)");
                    break;
                case 8:
                    Printf(" (DHCPInform)");
                    break;
              #else
                case 2:
                    adapter->DHCP_State = DHCP_OFFER;
                    break;
                case 5:
                    adapter->DHCP_State = DHCP_ACK;
                    break;
                case 6:
                    adapter->DHCP_State = DHCP_NAK;
                    break;
              #endif
            }
            break;
        case 54: // Server identifier
          #ifdef _NETWORK_DATA_
            for (i=0; i<opt[count+2]; i++)
            {
                Printf("%u ", opt[count+3+i]);
            }
          #endif

            if (opt[count+2] == 4)
            {
                MemCpy(adapter->Gateway_IP.IP4, opt+count+3, 4);
                arp_sendRequest(adapter, adapter->Gateway_IP); // send gateway IP/MAC to arp cache
            }
            break;
        default:
          #ifdef _NETWORK_DATA_
            for (i=0; i<opt[count+2]; i++)
                Printf("%u ", opt[count+3+i]);
          #endif
            break;
    }
    return (count + 2 + opt[count+2]);
}

static void DHCP_AnalyzeOptions(network_adapter_t* adapter, const uint8_t* opt)
{
    // check for magic number 63h 82h 53h 63h
    if (opt[0] != 0x63 || opt[1] != 0x82 || opt[2] != 0x53 || opt[3] != 0x63)
    {
        Printf("\nMAGIC NOT OK");
        return;
    }

    uint16_t count = 3;

    while (opt[count+1] != 0xFF) // no end token
    {
      #ifdef _NETWORK_DATA_
        switch (opt[count+1])
        {
        case 0:
            Printf("\nPadding");
            break;
        case 1:
            Printf("\nSubnet Mask: ");
            break;
        case 2:
            Printf("Time Offset: ");
            break;
        case 3:
            Printf("\nRouters: ");
            break;
        case 4:
            Printf("\nTime Server: ");
            break;
        case 5:
            Printf("\nName Server: ");
            break;
        case 6:
            Printf("\nDNS IP: ");
            break;
        case 7:
            Printf("\nLog Server: ");
            break;
        case 8:
            Printf("\nQuote Server: ");
            break;
        case 9:
            Printf("\nLPR Server: ");
            break;
        case 10:
            Printf("\nImpress Server: ");
            break;
        case 11:
            Printf("\nResource Location Server: ");
            break;
        case 12:
            Printf("\nHost Name: ");
            break;
        case 13:
            Printf("\nBoot File Size: ");
            break;
        case 14:
            Printf("\nMerit Dump File: ");
            break;
        case 15:
            Printf("\nDomain name: ");
            break;
        case 16:
            Printf("\nSwap Server: ");
            break;
        case 17:
            Printf("\nRoot Path: ");
            break;
        case 18:
            Printf("\nExtensions Path: ");
            break;
        case 19:
            Printf("\nIP Forwarding enable/disable: ");
            break;
        case 20:
            Printf("\nNon-local Source Routing enable/disable: ");
            break;
        case 21:
            Printf("\nPolicy Filter: ");
            break;
        case 22:
            Printf("\nMaximum Datagram Reassembly Size: ");
            break;
        case 23:
            Printf("\nDefault IP Time-to-live: ");
            break;
        case 24:
            Printf("\nPath MTU Aging Timeout: ");
            break;
        case 25:
            Printf("\nPath MTU Plateau Table:");
            break;
        case 26:
            Printf("\nInterface MTU: ");
            break;
        case 27:
            Printf("\nAll Subnets are Local: ");
            break;
        case 28:
            Printf("\nBroadcast Address: ");
            break;
        case 29:
            Printf("\nPerform Mask Discovery: ");
            break;
        case 30:
            Printf("\nMask supplier: ");
            break;
        case 31:
            Printf("\nPerform router discovery: ");
            break;
        case 32:
            Printf("\nRouter solicitation address: ");
            break;
        case 33:
            Printf("\nStatic routing table: ");
            break;
        case 34:
            Printf("\nTrailer encapsulation: ");
            break;
        case 35:
            Printf("\nARP cache timeout: ");
            break;
        case 36:
            Printf("\nEthernet encapsulation: ");
            break;
        case 37:
            Printf("\nDefault TCP TTL: ");
            break;
        case 38:
            Printf("\nTCP keepalive interval: ");
            break;
        case 39:
            Printf("\nTCP keepalive garbage: ");
            break;
        case 40:
            Printf("\nNetwork Information Service Domain: ");
            break;
        case 41:
            Printf("\nNetwork Information Servers: ");
            break;
        case 42:
            Printf("\nNTP servers: ");
            break;
        case 43:
            Printf("\nVendor specific info: ");
            break;
        case 44:
            Printf("\nNetBIOS over TCP/IP Name Server: ");
            break;
        case 45:
            Printf("\nNetBIOS over TCP/IP Datagram Distribution Server: ");
            break;
        case 46:
            Printf("\nNetBIOS over TCP/IP Node Type: ");
            break;
        case 47:
            Printf("\nNetBIOS over TCP/IP Scope: ");
            break;
        case 48:
            Printf("\nX Window System Font Server: ");
            break;
        case 49:
            Printf("\nX Window System Display Manager: ");
            break;
        case 50:
            Printf("\nRequested IP Address: ");
            break;
        case 51:
            Printf("\nIP address lease time: ");
            break;
        case 52:
            Printf("\nOption overload: ");
            break;
        case 53:
            Printf("\nMessage Type: ");
            break;
        case 54:
            Printf("\nServer IP: ");
            break;
        case 55:
            Printf("\nParameter request list: ");
            break;
        case 56:
            Printf("\nMessage: ");
            break;
        case 57:
            Printf("\nMaximum DHCP message size: ");
            break;
        case 58:
            Printf("\nRenew time value: ");
            break;
        case 59:
            Printf("\nRebinding time value: ");
            break;
        case 60:
            Printf("\nClass-identifier: ");
            break;
        case 61:
            Printf("\nClient-identifier: ");
            break;
        case 62:
            Printf("\nNetWare/IP Domain Name: ");
            break;
        case 63:
            Printf("\nNetWare/IP information: ");
            break;
        case 64:
            Printf("\nNetwork Information Service Domain: ");
            break;
        case 65:
            Printf("\nNetwork Information Service Servers:");
            break;
        case 66:
            Printf("\nTFTP server name: ");
            break;
        case 67:
            Printf("\nBootfile name: ");
            break;
        case 68:
            Printf("\nMobile IP Home Agent: ");
            break;
        case 69:
            Printf("\nSimple Mail Transport Protocol Server: ");
            break;
        case 70:
            Printf("\nPost Office Protocol Server: ");
            break;
        case 71:
            Printf("\nNetwork News Transport Protocol Server: ");
            break;
        case 72:
            Printf("\nDefault World Wide Web Server: ");
            break;
        case 73:
            Printf("\nDefault Finger Server: ");
            break;
        case 74:
            Printf("\nDefault Internet Relay Chat Server: ");
            break;
        case 75:
            Printf("\nStreetTalk Server: ");
            break;
        case 76:
            Printf("\nStreetTalk Directory Assistance Server: ");
            break;
        case 77:
            Printf("\nUser Class Information: ");
            break;
        case 78:
            Printf("\nSLP Directory Agent: ");
            break;
        case 79:
            Printf("\nSLP Service Scope: ");
            break;
        case 80:
            Printf("\nRapid Commit: ");
            break;
        case 81:
            Printf("\nFQDN, Fully Qualified Domain Name: ");
            break;
        case 82:
            Printf("\nRelay Agent Information: ");
            break;
        case 83:
            Printf("\nInternet Storage Name Service: ");
            break;
        case 85:
            Printf("\nNDS servers: ");
            break;
        case 86:
            Printf("\nNDS tree name: ");
            break;
        case 87:
            Printf("\nNDS context: ");
            break;
        case 88:
            Printf("\nBCMCS Controller Domain Name list: ");
            break;
        case 89:
            Printf("\nBCMCS Controller IPv4 address list: ");
            break;
        case 90:
            Printf("\nAuthentication: ");
            break;
        case 91:
            Printf("\nclient-last-transaction-time: ");
            break;
        case 92:
            Printf("\nassociated-ip: ");
            break;
        case 93:
            Printf("\nClient System Architecture Type: ");
            break;
        case 94:
            Printf("\nClient Network Interface Identifier: ");
            break;
        case 95:
            Printf("\nLDAP, Lightweight Directory Access Protocol: ");
            break;
        case 97:
            Printf("\nClient Machine Identifier: ");
            break;
        case 98:
            Printf("\nOpen Group's User Authentication: ");
            break;
        case 99:
            Printf("\nGEOCONF_CIVIC: ");
            break;
        case 100:
            Printf("\nIEEE 1003.1 TZ String: ");
            break;
        default:
            Printf("\nUnknown option: %u", opt[count+1]);
            break;
        } //switch
      #endif

        if (opt[count+1] != 0)
            count = showOptionsBytes(adapter, opt, count);
        else count++; // Padding
    } //while
  #ifdef _NETWORK_DATA_
    Printf("\nEND OF OPTIONS\n");
  #endif
}


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
