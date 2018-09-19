
#include "netutils.h"


typedef struct
{
    IP4_t src;
    IP4_t dest;
    uint8_t res;
    uint8_t prot;
    uint16_t length;
} __attribute__((packed)) updtcpPseudoHeader_t;


uint16_t internetChecksum(const void* addr, size_t count, uint32_t pseudoHeaderChecksum)
{
    uint32_t sum  = pseudoHeaderChecksum;
    const uint8_t* data = addr;

    while (count > 1) // inner loop
    {
        sum   += (data[0] << 8) | data[1]; // Big Endian
        data  += 2;
        count -= 2;
    }

    if (count > 0) // add left-over byte, if any
    {
        sum += data[0] << 8;
    }

    while (sum >> 16) // fold 32-bit sum to 16 bits
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum & 0xFFFF;
}

uint16_t udptcpCalculateChecksum(const void* p, uint16_t length, IP4_t srcIP, IP4_t destIP, uint16_t protocol)
{
    updtcpPseudoHeader_t pseudo;
    pseudo.src.iIP4 = srcIP.iIP4;
    pseudo.dest.iIP4 = destIP.iIP4;
    pseudo.length = htons(length);
    pseudo.prot = protocol;
    pseudo.res = 0;

    uint32_t pseudoHeaderChecksum = 0;
    const uint8_t* data = (uint8_t*)&pseudo;

    uint8_t i;
    for (i = 0; i < sizeof(updtcpPseudoHeader_t); i+=2)
    {
        // pseudo header contains 6 WORD
        pseudoHeaderChecksum += (data[i] << 8u) | data[i+1]; // Big Endian
    }

    return internetChecksum(p, length, pseudoHeaderChecksum);
}

bool sameSubnet(IP4_t IP1, IP4_t IP2, IP4_t subnet)
{
    return ((IP1.iIP4 & subnet.iIP4) == (IP2.iIP4 & subnet.iIP4));
}

