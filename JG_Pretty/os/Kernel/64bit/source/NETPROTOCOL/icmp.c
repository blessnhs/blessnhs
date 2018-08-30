/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss f�r die Verwendung dieses Sourcecodes siehe unten
*/

#include "icmp.h"
#include "ipv4.h"

void icmp_sendEchoRequest(network_adapter_t* adapter, IP4_t destIP)
{
    static uint16_t count = 0;
    count++;

    static const char data[] = "PrettyOS ist das Betriebssystem der Projektgruppe \"OS-Development\" im deutschsprachigen C++-Forum";
    size_t packetSize = sizeof(icmpheader_t) + strlen(data) + 1;
    char icmpPacket[packetSize];

    icmpheader_t* icmp = (void*)icmpPacket;
    MemCmp((char*)(icmp+1), data,strlen(data));
    icmp->type         = 8; // echo request
    icmp->code         = 0;
    icmp->id           = htons(0xAFFE);
    icmp->seqnumber    = htons(count);
    icmp->checksum     = 0;
    icmp->checksum     = htons(internetChecksum(icmp, packetSize, 0));

    ipv4_send(adapter, icmp, packetSize, destIP, 1, 0);
}

void icmp_receive(network_adapter_t* adapter, const icmpheader_t* rec, uint32_t length, IP4_t sourceIP)
{
    size_t icmp_data_length = length - sizeof(icmpheader_t);

    switch (rec->type)
    {
        case ICMP_ECHO_REPLY:
            Printf("Echo Reply:");

            if (rec->code == 0)
            {
                Printf("  ID: %x seq: %u\n", ntohs(rec->id), ntohs(rec->seqnumber));
               // memshow((const char*)(rec + 1), icmp_data_length, true);
            }
            break;

      #ifdef _ICMP_DEBUG_
        case ICMP_DESTINATION_UNREACHABLE:
            textColor(ERROR);
            Printf("Destination unreachable - code %u", rec->code);
            switch (rec->code)
            {
                case 0:
                    Printf(": Destination network unreachable");
                    break;
                case 1:
                    Printf(": Destination host unreachable");
                    break;
                case 2:
                    Printf(": Destination protocol unreachable");
                    break;
                case 3:
                    Printf(": Destination port unreachable");
                    break;
                case 4:
                    Printf(": Fragmentation required, and DF flag set");
                    break;
                case 5:
                    Printf(": Source route failed");
                    break;
                case 6:
                    Printf(": Destination network unknown");
                    break;
                case 7:
                    Printf(": Destination host unknown");
                    break;
                case 8:
                    Printf(": Source host isolated");
                    break;
                case 9:
                    Printf(": Network administratively prohibited");
                    break;
                case 10:
                    Printf(": Host administratively prohibited");
                    break;
                case 11:
                    Printf(": Network unreachable for TOS");
                    break;
                case 12:
                    Printf(": Host unreachable for TOS");
                    break;
                case 13:
                    Printf(": Communication administratively prohibited");
                    break;
            }
            textColor(TEXT);
            break;
        case ICMP_SOURCE_QUENCH:
            Printf("Source quench (congestion control)");
            break;
        case ICMP_REDIRECT:
            Printf("Redirect - code %u", rec->code);
            switch (rec->code)
            {
                case 0:
                    Printf(": Redirect Datagram for the Network");
                    break;
                case 1:
                    Printf(": Redirect Datagram for the Host");
                    break;
                case 2:
                    Printf(": Redirect Datagram for the TOS & network");
                    break;
                case 3:
                    Printf(": Redirect Datagram for the TOS & host");
                    break;
            }
            break;
        case 6:
            Printf("Alternate Host Address");
            break;
      #endif

        case ICMP_ECHO_REQUEST:
        {
            Printf("ICMP_echoRequest:");

            uint8_t pkt[sizeof(icmpheader_t) + icmp_data_length];

            icmpheader_t* icmp = (icmpheader_t*)pkt;

            icmp->type         = ICMP_ECHO_REPLY;
            icmp->code         = 0;
            icmp->id           = rec->id;
            icmp->seqnumber    = rec->seqnumber;
            icmp->checksum     = 0;

            MemCpy(icmp+1, rec+1, icmp_data_length);

            icmp->checksum = htons(internetChecksum(icmp, sizeof(icmpheader_t) + icmp_data_length, 0));

            Printf(" type: %u  code: %u  checksum %u\n", icmp->type, icmp->code, icmp->checksum);

            ipv4_send(adapter, (void*)icmp, sizeof(icmpheader_t) + icmp_data_length, sourceIP, 1, 0);
            break;
        }

      #ifdef _ICMP_DEBUG_
        case ICMP_ROUTER_ADVERTISEMENT:
            Printf("Router Advertisement");
            break;
        case ICMP_ROUTER_SOLICITATION:
            Printf("Router discovery/selection/solicitation");
            break;
        case ICMP_TIME_EXEEDED:
            textColor(ERROR);
            Printf("Time Exceeded - code %u", rec->code);
            switch (rec->code)
            {
                case 0:
                    Printf(": TTL expired in transit");
                    break;
                case 1:
                    Printf(": Fragment reassembly time exceeded");
                    break;
            }
            textColor(TEXT);
            break;
        case ICMP_PARAMETER_PROBLEM:
            textColor(ERROR);
            Printf("Parameter Problem: Bad IP header - code %u", rec->code);
            switch (rec->code)
            {
                case 0:
                    Printf(": Pointer indicates the error");
                    break;
                case 1:
                    Printf(": Missing a required option");
                    break;
            }
            textColor(TEXT);
            break;
        case ICMP_TIMESTAMP:
            Printf("Timestamp");
            break;
        case ICMP_TIMESTAMP_REPLY:
            Printf("Timestamp reply");
            break;
        case ICMP_INFORMATION_REQUEST:
            Printf("Information Request");
            break;
        case ICMP_INFORMATION_REPLY:
            Printf("Information Reply");
            break;
        case ICMP_ADDRESS_MASK_REQUEST:
            Printf("Address Mask Request");
            break;
        case ICMP_ADDRESS_MASK_REPLY:
            Printf("Address Mask Reply");
            break;
        case ICMP_TRACEROUTE:
            Printf("Information Request");
            break;
        case ICMP_DATAGRAM_CONVERSION_ERROR:
            Printf("Datagram Conversion Error");
            break;
        case ICMP_MOBILE_HOST_REDIRECT:
            Printf("Mobile Host Redirect");
            break;
        case ICMP_WHERE_ARE_YOU:
            Printf("Where-Are-You (originally meant for IPv6)");
            break;
        case ICMP_I_AM_HERE:
            Printf("Here-I-Am (originally meant for IPv6)");
            break;
        case ICMP_MOBILE_REGISTRATION_REQUEST:
            Printf("Mobile Registration Request");
            break;
        case ICMP_MOBILE_REGISTRATION_REPLY:
            Printf("Mobile Registration Reply");
            break;
        case ICMP_DOMAIN_NAME_REQUEST:
            Printf("Domain Name Request");
            break;
        case ICMP_DOMAIN_NAME_REPLY:
            Printf("Domain Name Reply");
            break;
        case ICMP_SKIP:
            Printf("SKIP Algorithm Discovery Protocol, Simple Key-Management for Internet Protocol");
            break;
        case ICMP_PHOTURIS:
            Printf("Photuris, Security failures");
            break;
        case ICMP_SEAMOBY:
            Printf("ICMP for experimental mobility protocols such as Seamoby [RFC4065]");
            break;
      #endif
        default:
            break;
    }
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
