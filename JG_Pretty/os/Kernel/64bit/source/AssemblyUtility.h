
#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

////////////////////////////////////////////////////////////////////////////////
//
//  ÇÔ¼ö
//
////////////////////////////////////////////////////////////////////////////////
BYTE InPortByte( WORD wPort );
void OutPortByte( WORD wPort, BYTE bData );

WORD InPortWord( WORD wPort );
void OutPortWord( WORD wPort, WORD wData );

DWORD InPortDWord( WORD wPort );
void OutPortDWord( WORD wPort, DWORD wData );

void LoadGDTR( QWORD qwGDTRAddress );
void LoadTR( WORD wTSSSegmentOffset );
void LoadIDTR( QWORD qwIDTRAddress);
void EnableInterrupt( void );
void DisableInterrupt( void );
QWORD ReadRFLAGS( void );
QWORD ReadTSC( void );
void SwitchContext( CONTEXT* pstCurrentContext, CONTEXT* pstNextContext );
void Hlt( void );
BOOL CMPXCHG( volatile BYTE* pbDestination, BYTE bCompare, BYTE bSource );
void InitializeFPU( void );
void SaveFPUContext( void* pvFPUContext );
void LoadFPUContext( void* pvFPUContext );
void SetTS( void );
void ClearTS( void );
void EnableGlobalLocalAPIC( void );
void Pause( void );
void ReadMSR( QWORD qwMSRAddress, QWORD* pqwRDX, QWORD* pqwRAX );
void WriteMSR( QWORD qwMSRAddress, QWORD qwRDX, QWORD qwRAX );
unsigned long long ReadCr3();
#endif /*__ASSEMBLYUTILITY_H__*/
