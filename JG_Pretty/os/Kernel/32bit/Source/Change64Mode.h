 
#ifndef __MODESWITCH_H__
#define __MODESWITCH_H__

#include  "Types.h"

void ReadCPUID( DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX,
        DWORD* pdwEDX );
void Change64BitKernel( void );

#endif /*__MODESWITCH_H__*/
