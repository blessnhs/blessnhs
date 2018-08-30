/**
 *  file    Descriptor.c
 *  date    2009/01/16
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GDT 및 IDT에 관련된 각종 디스크립터에 대한 파일
 */

#include "Descriptor.h"
#include "Utility.h"
#include "ISR.h"
#include "MultiProcessor.h"

//==============================================================================
//  GDT 및 TSS
//==============================================================================

/**
 *  GDT 테이블을 초기화
 */
void InitializeGDTTableAndTSS( void )
{
    GDTR* pstGDTR;
    GDTENTRY8* pstEntry;
    TSSSEGMENT* pstTSS;
    int i;
    
    // GDTR 설정
    pstGDTR = ( GDTR* ) GDTR_STARTADDRESS;
    pstEntry = ( GDTENTRY8* ) ( GDTR_STARTADDRESS + sizeof( GDTR ) );
    pstGDTR->wLimit = GDT_TABLESIZE - 1;
    pstGDTR->qwBaseAddress = ( QWORD ) pstEntry;
    
    // TSS 세그먼트 영역 설정, GDT 테이블의 뒤쪽에 위치
    pstTSS = ( TSSSEGMENT* ) ( ( QWORD ) pstEntry + GDT_TABLESIZE );

   // NULL, 커널 코드/데이터, 유저 코드/데이터, TSS를 위해 총 5 + 16개의 세그먼트를 생성
    SetGDTEntry8( &( pstEntry[ 0 ] ), 0, 0, 0, 0, 0 );
    // 커널 레벨 코드/데이터 디스크립터 생성
    SetGDTEntry8( &( pstEntry[ 1 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, 
            GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE );
    SetGDTEntry8( &( pstEntry[ 2 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA,
            GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA );
    // 유저 레벨 코드/데이터 디스크립터 생성
    SetGDTEntry8( &( pstEntry[ 3 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA,
            GDT_FLAGS_LOWER_USERDATA, GDT_TYPE_DATA );
    SetGDTEntry8( &( pstEntry[ 4 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, 
            GDT_FLAGS_LOWER_USERCODE, GDT_TYPE_CODE );
    
    // 16개 코어 지원을 위해 16개의 TSS 디스크립터를 생성
    for( i = 0 ; i < MAXPROCESSORCOUNT ; i++ )
    {
        // TSS는 16바이트이므로, SetGDTEntry16() 함수 사용
        // pstEntry는 8바이트이므로 2개를 합쳐서 하나로 사용
        SetGDTEntry16( ( GDTENTRY16* ) &( pstEntry[ GDT_MAXENTRY8COUNT + 
                ( i * 2 ) ] ), ( QWORD ) pstTSS + ( i * sizeof( TSSSEGMENT ) ), 
                sizeof( TSSSEGMENT ) - 1, GDT_FLAGS_UPPER_TSS, 
                GDT_FLAGS_LOWER_TSS, GDT_TYPE_TSS ); 
    }
    
    // TSS 초기화, GDT 이하 영역을 사용함
    InitializeTSSSegment( pstTSS );    
}

/**
 *  8바이트 크기의 GDT 엔트리에 값을 설정
 *      코드와 데이터 세그먼트 디스크립터를 설정하는데 사용
 */
void SetGDTEntry8( GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
    pstEntry->wLowerLimit = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;
    pstEntry->bUpperBaseAddress1 = ( dwBaseAddress >> 16 ) & 0xFF;
    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) | 
        bUpperFlags;
    pstEntry->bUpperBaseAddress2 = ( dwBaseAddress >> 24 ) & 0xFF;
}

/**
 *  16바이트 크기의 GDT 엔트리에 값을 설정
 *      TSS 세그먼트 디스크립터를 설정하는데 사용
 */
void SetGDTEntry16( GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
    pstEntry->wLowerLimit = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;
    pstEntry->bMiddleBaseAddress1 = ( qwBaseAddress >> 16 ) & 0xFF;
    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) | 
        bUpperFlags;
    pstEntry->bMiddleBaseAddress2 = ( qwBaseAddress >> 24 ) & 0xFF;
    pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;
    pstEntry->dwReserved = 0;
}

/**
 *  TSS 세그먼트의 정보를 초기화
 */
void InitializeTSSSegment( TSSSEGMENT* pstTSS )
{
    int i;
    
    // 최대 프로세서 또는 코어의 수만큼 루프를 돌면서 생성
    for( i = 0 ; i < MAXPROCESSORCOUNT ; i++ )
    {
        // 0으로 초기화
        MemSet( pstTSS, 0, sizeof( TSSSEGMENT ) );

        // IST의 뒤에서부터 잘라서 할당함. (주의, IST는 16바이트 단위로 정렬해야 함)
        pstTSS->qwIST[ 0 ] = IST_STARTADDRESS + IST_SIZE - 
            ( IST_SIZE / MAXPROCESSORCOUNT * i );
        
        // IO Map의 기준 주소를 TSS 디스크립터의 Limit 필드보다 크게 설정함으로써 
        // IO Map을 사용하지 않도록 함
        pstTSS->wIOMapBaseAddress = 0xFFFF;

        // 다음 엔트리로 이동
        pstTSS++;
    }
}


//==============================================================================
//  IDT
//==============================================================================
/**
 *  IDT 테이블을 초기화
 */
void InitializeIDTTables( void )
{
    IDTR* pstIDTR;
    IDTENTRY* pstEntry;
    int i;
        
    // IDTR의 시작 어드레스
    pstIDTR = ( IDTR* ) IDTR_STARTADDRESS;
    // IDT 테이블의 정보 생성
    pstEntry = ( IDTENTRY* ) ( IDTR_STARTADDRESS + sizeof( IDTR ) );
    pstIDTR->qwBaseAddress = ( QWORD ) pstEntry;
    pstIDTR->wLimit = IDT_TABLESIZE - 1;
    
    //==========================================================================
    // 예외 ISR 등록
    //==========================================================================
    SetIDTEntry( &( pstEntry[ 0 ] ), ISRDivideError, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 1 ] ), ISRDebug, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 2 ] ), ISRNMI, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 3 ] ), ISRBreakPoint, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 4 ] ), ISROverflow, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 5 ] ), ISRBoundRangeExceeded, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 6 ] ), ISRInvalidOpcode, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 7 ] ), ISRDeviceNotAvailable, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 8 ] ), ISRDoubleFault, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 9 ] ), ISRCoprocessorSegmentOverrun, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 10 ] ), ISRInvalidTSS, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 11 ] ), ISRSegmentNotPresent, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 12 ] ), ISRStackSegmentFault, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 13 ] ), ISRGeneralProtection, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 14 ] ), ISRPageFault, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 15 ] ), ISR15, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 16 ] ), ISRFPUError, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 17 ] ), ISRAlignmentCheck, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 18 ] ), ISRMachineCheck, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 19 ] ), ISRSIMDError, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 20 ] ), ISRETCException, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    
    for( i = 21 ; i < 32 ; i++ )
    {
        SetIDTEntry( &( pstEntry[ i ] ), ISRETCException, 0x08, IDT_FLAGS_IST1,
            IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    }
    //==========================================================================
    // 인터럽트 ISR 등록
    //==========================================================================
    SetIDTEntry( &( pstEntry[ 32 ] ), ISRTimer, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 33 ] ), ISRKeyboard, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 34 ] ), ISRSlavePIC, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 35 ] ), ISRSerial2, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 36 ] ), ISRSerial1, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 37 ] ), ISRParallel2, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 38 ] ), ISRFloppy, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 39 ] ), ISRParallel1, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 40 ] ), ISRRTC, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 41 ] ), ISRReserved, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 42 ] ), ISRNotUsed1, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 43 ] ), ISRNotUsed2, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 44 ] ), ISRMouse, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 45 ] ), ISRCoprocessor, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 46 ] ), ISRHDD1, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    SetIDTEntry( &( pstEntry[ 47 ] ), ISRHDD2, 0x08, IDT_FLAGS_IST1,
        IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );

    for( i = 48 ; i < IDT_MAXENTRYCOUNT ; i++ )
    {
        SetIDTEntry( &( pstEntry[ i ] ), ISRETCInterrupt, 0x08, IDT_FLAGS_IST1,
            IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    }
}

/**
 *  IDT 게이트 디스크립터에 값을 설정
 */
void SetIDTEntry( IDTENTRY* pstEntry, void* pvHandler, WORD wSelector, 
        BYTE bIST, BYTE bFlags, BYTE bType )
{
    pstEntry->wLowerBaseAddress = ( QWORD ) pvHandler & 0xFFFF;
    pstEntry->wSegmentSelector = wSelector;
    pstEntry->bIST = bIST & 0x3;
    pstEntry->bTypeAndFlags = bType | bFlags;
    pstEntry->wMiddleBaseAddress = ( ( QWORD ) pvHandler >> 16 ) & 0xFFFF;
    pstEntry->dwUpperBaseAddress = ( QWORD ) pvHandler >> 32;
    pstEntry->dwReserved = 0;
}
