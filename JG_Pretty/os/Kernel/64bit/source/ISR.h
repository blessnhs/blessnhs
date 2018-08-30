/**
 *  file    ISR.h
 *  date    2009/01/24
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   인터럽트 서비스 루틴(ISR) 관련된 헤더 파일
 */

#ifndef __ISR_H__
#define __ISR_H__

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
// 예외(Exception) 처리용 ISR
void ISRDivideError( void );
void ISRDebug( void );
void ISRNMI( void );
void ISRBreakPoint( void );
void ISROverflow( void );
void ISRBoundRangeExceeded( void );
void ISRInvalidOpcode();
void ISRDeviceNotAvailable( void );
void ISRDoubleFault( void );
void ISRCoprocessorSegmentOverrun( void );
void ISRInvalidTSS( void );
void ISRSegmentNotPresent( void );
void ISRStackSegmentFault( void );
void ISRGeneralProtection( void );
void ISRPageFault( void );
void ISR15( void );
void ISRFPUError( void );
void ISRAlignmentCheck( void );
void ISRMachineCheck( void );
void ISRSIMDError( void );
void ISRETCException( void );

// 인터럽트(Interrupt) 처리용 ISR
void ISRTimer( void );
void ISRKeyboard( void );
void ISRSlavePIC( void );
void ISRSerial2( void );
void ISRSerial1( void );
void ISRParallel2( void );
void ISRFloppy( void );
void ISRParallel1( void );
void ISRRTC( void );
void ISRReserved( void );
void ISRNotUsed1( void );
void ISRNotUsed2( void );
void ISRMouse( void );
void ISRCoprocessor( void );
void ISRHDD1( void );
void ISRHDD2( void );
void ISRETCInterrupt( void );

#endif /*__ISR_H__*/
