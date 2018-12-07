#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdarg.h>
#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
//  매크로
//
////////////////////////////////////////////////////////////////////////////////
#define MIN( x, y )     ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MAX( x, y )     ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
void MemSet( void* pvDestination, BYTE bData, int iSize );
inline void MemSetWord( void* pvDestination, WORD wData, int iWordSize );
int MemCpy( void* pvDestination, const void* pvSource, int iSize );
int memcpy( void* pvDestination, const void* pvSource, int iSize );
int MemCmp( const void* pvDestination, const void* pvSource, int iSize );
void* memset(void* dest, char val, size_t bytes);

BOOL SetInterruptFlag( BOOL bEnableInterrupt );
void CheckTotalRAMSize( void );
QWORD GetTotalRAMSize( void );
void ReverseString( char* pcBuffer );
long AToI( const char* pcBuffer, int iRadix );
QWORD HexStringToQword( const char* pcBuffer );
long DecimalStringToLong( const char* pcBuffer );
int IToA( long lValue, char* pcBuffer, int iRadix );
int HexToString( QWORD qwValue, char* pcBuffer );
int DecimalToString( long lValue, char* pcBuffer );
int SPrintf( char* pcBuffer, const char* pcFormatString, ... );
int VSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap );
QWORD GetTickCount( void );
void Sleep( QWORD qwMillisecond );
BOOL IsGraphicMode( void );
void *MemChr(const void *buf, int c, int n);
char* strcpy(char* dest, const char* src);
char* strchr(const char* str, int character);
long int strtol(const char* nptr, char** endptr, int base);
int atoi(const char* nptr);
int isspace(int c);
void memshow(const void* start, size_t count, bool alpha);
char *strstr(char *string, char *substring);
unsigned long long *GetPhysAddr(void *virtualaddr);
unsigned long long *GetEntryLevel(unsigned long long address, int level);
int32_t strcmp(const char* s1, const char* s2);
int strncmp (const char *cs, const char *ct, size_t count);
unsigned int alignUp(unsigned int val, unsigned int alignment);
unsigned int alignDown(unsigned int val, unsigned int alignment);
size_t strlen(const char* str);
int bsr(int val);
uint32_t* memsetl(uint32_t* dest, uint32_t val, size_t dwords);
char* strcat(char* dest, const char* src);

static inline void nop(void) { __asm__ volatile ("nop"); } // Do nothing
static inline void hlt(void) { __asm__ volatile ("hlt"); } // Wait until next interrupt
static inline void sti(void) { __asm__ volatile ("sti"); } // Enable interrupts
static inline void cli(void) { __asm__ volatile ("cli"); } // Disable interrupts

#define WAIT_FOR_CONDITION(condition, runs, wait, message)\
{\
	unsigned int timeout_;\
    for (timeout_ = 0; !(condition); timeout_++) {\
        if (timeout_ >= runs) {\
             Printf(message);\
             break;\
        }\
        WaitUsingDirectPIT(wait);\
    }\
}

#define BIT(n) (1U<<(n))

// hton = Host To Network
uint16_t htons(uint16_t v);
uint32_t htonl(uint32_t v);

// ntoh = Network To Host
#define ntohs(v) htons(v)
#define ntohl(v) htonl(v)

#define swap16(x) ({unsigned _x = (x); (((_x)>>8)&0xff) | (((_x)<<8)&0xff00); })

#define swap32(x) ({\
	unsigned _x = (x); \
	(((_x)>>24)&0xff)\
	|\
	(((_x)>>8)&0xff00)\
	|\
	(((_x)<<8)&0xff0000)\
	|\
	(((_x)<<24)&0xff000000);\
})

#define swap64(_v) (((unsigned long)swap32((unsigned)(_v))<<32)|(unsigned long)swap32((unsigned)((_v)>>32)))
////////////////////////////////////////////////////////////////////////////////
//
//  기타
//
////////////////////////////////////////////////////////////////////////////////
extern volatile QWORD g_qwTickCount;

#endif /*__UTILITY_H__*/
