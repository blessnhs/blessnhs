/**
 *  file    MPConfigurationTable.c
 *  date    2009/06/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   MP 설정 테이블(MP Configuration Table)에 관련된 소스 파일
 */

#include "MPConfigurationTable.h"
#include "Console.h"


// MP 설정 테이블을 관리하는 자료구조
static MPCONFIGRUATIONMANAGER gs_stMPConfigurationManager = { 0, };

/**
 *  BIOS의 영역에서 MP Floating Header를 찾아서 그 주소를 반환
 */
BOOL FindMPFloatingPointerAddress( QWORD* pstAddress )
{
    char* pcMPFloatingPointer;
    QWORD qwEBDAAddress;
    QWORD qwSystemBaseMemory;

    // 확장 BIOS 데이터 영역과 시스템 기본 메모리를 출력
    //Printf( "Extended BIOS Data Area = [0x%X] \n", 
    //        ( DWORD ) ( *( WORD* ) 0x040E ) * 16 );
    //Printf( "System Base Address = [0x%X]\n", 
    //        ( DWORD ) ( *( WORD* ) 0x0413 ) * 1024 );
    
    // 확장 BIOS 데이터 영역을 검색하여 MP 플로팅 포인터를 찾음
    // 확장 BIOS 데이터 영역은 0x040E에서 세그먼트의 시작 어드레스를 찾을 수 있음
    qwEBDAAddress = *( WORD* ) ( 0x040E );
    // 세그먼트의 시작 어드레스이므로 16을 곱하여 실제 물리 어드레스로 변환
    qwEBDAAddress *= 16;
    
    for( pcMPFloatingPointer = ( char* ) qwEBDAAddress ; 
         ( QWORD ) pcMPFloatingPointer <= ( qwEBDAAddress + 1024 ) ; 
         pcMPFloatingPointer++ )
    {
        if( MemCmp( pcMPFloatingPointer, "_MP_", 4 ) == 0 )
        {
            //Printf( "MP Floating Pointer Is In EBDA, [0x%X] Address\n", 
            //         ( QWORD ) pcMPFloatingPointer );
            *pstAddress = ( QWORD ) pcMPFloatingPointer;
            return 1;
        }
    }

    // 시스템 기본 메모리의 끝부분에서 1Kbyte 미만인 영역을 검색하여 MP 플로팅 포인터를
    // 찾음
    // 시스템 기본 메모리는 0x0413에서 Kbyte 단위로 정렬된 값을 찾을 수 있음
    qwSystemBaseMemory = *( WORD* ) 0x0413;
    // Kbyte 단위로 저장된 값이므로 1024를 곱해 실제 물리 어드레스로 변환
    qwSystemBaseMemory *= 1024;

    for( pcMPFloatingPointer = ( char* ) ( qwSystemBaseMemory - 1024 ) ; 
        ( QWORD ) pcMPFloatingPointer <= qwSystemBaseMemory ; 
        pcMPFloatingPointer++ )
    {
        if( MemCmp( pcMPFloatingPointer, "_MP_", 4 ) == 0 )
        {
            //Printf( "MP Floating Pointer Is In System Base Memory, [0x%X] Address\n",
            //         ( QWORD ) pcMPFloatingPointer );
            *pstAddress = ( QWORD ) pcMPFloatingPointer;
            return 1;
        }
    }
    
    // BIOS의 ROM 영역을 검색하여 MP 플로팅 포인터를 찾음
    for( pcMPFloatingPointer = ( char* ) 0x0F0000; 
         ( QWORD) pcMPFloatingPointer < 0x0FFFFF; pcMPFloatingPointer++ )
    {
        if( MemCmp( pcMPFloatingPointer, "_MP_", 4 ) == 0 )
        {
            //Printf( "MP Floating Pointer Is In ROM, [0x%X] Address\n", 
            //         pcMPFloatingPointer );
            *pstAddress = ( QWORD ) pcMPFloatingPointer;
            return TRUE;
        }
    }

    return 0;
}

/**
 *  MP 설정 테이블을 분석해서 필요한 정보를 저장하는 함수
 */
BOOL AnalysisMPConfigurationTable( void )
{
    QWORD qwMPFloatingPointerAddress;
    MPFLOATINGPOINTER* pstMPFloatingPointer;
    MPCONFIGURATIONTABLEHEADER* pstMPConfigurationHeader;
    BYTE bEntryType;
    WORD i;
    QWORD qwEntryAddress;
    PROCESSORENTRY* pstProcessorEntry;
    BUSENTRY* pstBusEntry;
    
    // 자료구조 초기화
    MemSet( &gs_stMPConfigurationManager, 0, sizeof( MPCONFIGRUATIONMANAGER ) );
    gs_stMPConfigurationManager.bISABusID = 0xFF;
    
    // MP 플로팅 포인터의 어드레스를 구함
    if( FindMPFloatingPointerAddress( &qwMPFloatingPointerAddress ) == 0 )
    {
        return 0;
    }
    
    // MP 플로팅 테이블 설정
    pstMPFloatingPointer = ( MPFLOATINGPOINTER* ) qwMPFloatingPointerAddress;
    gs_stMPConfigurationManager.pstMPFloatingPointer = pstMPFloatingPointer;
    pstMPConfigurationHeader = ( MPCONFIGURATIONTABLEHEADER* ) 
        ( ( QWORD ) pstMPFloatingPointer->dwMPConfigurationTableAddress & 0xFFFFFFFF );
    
    // PIC 모드 지원 여부 저장
    if( pstMPFloatingPointer->vbMPFeatureByte[ 1 ] & 
            MP_FLOATINGPOINTER_FEATUREBYTE2_PICMODE )
    {
        gs_stMPConfigurationManager.bUsePICMode = 1;
    }    
    
    // MP 설정 테이블 헤더와 기본 MP 설정 테이블 엔트리의 시작 어드레스 설정
    gs_stMPConfigurationManager.pstMPConfigurationTableHeader = 
        pstMPConfigurationHeader;
    gs_stMPConfigurationManager.qwBaseEntryStartAddress = 
        pstMPFloatingPointer->dwMPConfigurationTableAddress + 
        sizeof( MPCONFIGURATIONTABLEHEADER );
    
    // 모든 엔트리를 돌면서 프로세서의 코어 수를 계산하고 ISA 버스를 검색하여 ID를 저장
    qwEntryAddress = gs_stMPConfigurationManager.qwBaseEntryStartAddress;
    for( i = 0 ; i < pstMPConfigurationHeader->wEntryCount ; i++ )
    {
        bEntryType = *( BYTE* ) qwEntryAddress;
        switch( bEntryType )
        {
            // 프로세서 엔트리이면 프로세서의 수를 하나 증가시킴
        case MP_ENTRYTYPE_PROCESSOR:
            pstProcessorEntry = ( PROCESSORENTRY* ) qwEntryAddress;
            if( pstProcessorEntry->bCPUFlags & MP_PROCESSOR_CPUFLAGS_ENABLE )
            {
                gs_stMPConfigurationManager.iProcessorCount++;
            }
            qwEntryAddress += sizeof( PROCESSORENTRY );
            break;
            
            // 버스 엔트리이면 ISA 버스인지 확인하여 저장
        case MP_ENTRYTYPE_BUS:
            pstBusEntry = ( BUSENTRY* ) qwEntryAddress;
            if( MemCmp( pstBusEntry->vcBusTypeString, MP_BUS_TYPESTRING_ISA,
                    kStrLen( MP_BUS_TYPESTRING_ISA ) ) == 0 )
            {
                gs_stMPConfigurationManager.bISABusID = pstBusEntry->bBusID;
            }
            qwEntryAddress += sizeof( BUSENTRY );
            break;
            
            // 기타 엔트리는 무시하고 이동
        case MP_ENTRYTYPE_IOAPIC:
        case MP_ENTRYTYPE_IOINTERRUPTASSIGNMENT:
        case MP_ENTRYTYPE_LOCALINTERRUPTASSIGNMENT:
        default:
            qwEntryAddress += 8;
            break;
        }
    }
    return TRUE;
}

/**
 *  MP 설정 테이블을 관리하는 자료구조를 반환
 */
MPCONFIGRUATIONMANAGER* kGetMPConfigurationManager( void )
{
    return &gs_stMPConfigurationManager;
}

/**
 *  MP 설정 테이블의 정보를 모두 화면에 출력
 */
void PrintMPConfigurationTable( void )
{
    MPCONFIGRUATIONMANAGER* pstMPConfigurationManager;
    QWORD qwMPFloatingPointerAddress;
    MPFLOATINGPOINTER* pstMPFloatingPointer;
    MPCONFIGURATIONTABLEHEADER* pstMPTableHeader;
    PROCESSORENTRY* pstProcessorEntry;
    BUSENTRY* pstBusEntry;
    IOAPICENTRY* pstIOAPICEntry;
    IOINTERRUPTASSIGNMENTENTRY* pstIOAssignmentEntry;
    LOCALINTERRUPTASSIGNMENTENTRY* pstLocalAssignmentEntry;
    QWORD qwBaseEntryAddress;
    char vcStringBuffer[ 20 ];
    WORD i;
    BYTE bEntryType;
    // 화면에 출력할 문자열
    char* vpcInterruptType[ 4 ] = { "INT", "NMI", "SMI", "ExtINT" };
    char* vpcInterruptFlagsPO[ 4 ] = { "Conform", "Active High", 
            "Reserved", "Active Low" };
    char* vpcInterruptFlagsEL[ 4 ] = { "Conform", "Edge-Trigger", "Reserved", 
            "Level-Trigger"};

    //==========================================================================
    // MP 설정 테이블 처리 함수를 먼저 호출하여 시스템 처리에 필요한 정보를 저장
    //==========================================================================
    Printf( "================ MP Configuration Table Summary ================\n" );
    pstMPConfigurationManager = kGetMPConfigurationManager();
    if( ( pstMPConfigurationManager->qwBaseEntryStartAddress == 0 ) &&
        ( AnalysisMPConfigurationTable() == 0 ) )
    {
        Printf( "MP Configuration Table Analysis Fail\n" );
        return ;
    }
    Printf( "MP Configuration Table Analysis Success\n" );
    
    Printf( "MP Floating Pointer Address : 0x%Q\n", 
            pstMPConfigurationManager->pstMPFloatingPointer );
    Printf( "PIC Mode Support : %d\n", pstMPConfigurationManager->bUsePICMode );
    Printf( "MP Configuration Table Header Address : 0x%Q\n",
            pstMPConfigurationManager->pstMPConfigurationTableHeader );
    Printf( "Base MP Configuration Table Entry Start Address : 0x%Q\n",
            pstMPConfigurationManager->qwBaseEntryStartAddress );
    Printf( "Processor Count : %d\n", pstMPConfigurationManager->iProcessorCount );
    Printf( "ISA Bus ID : %d\n", pstMPConfigurationManager->bISABusID );

    Printf( "Press any key to continue... ('q' is exit) : " );
    if( GetCh() == 'q' )
    {
        Printf( "\n" );
        return ;
    }
    Printf( "\n" );            
    
    //==========================================================================
    // MP 플로팅 포인터 정보를 출력
    //==========================================================================
    Printf( "=================== MP Floating Pointer ===================\n" );
    pstMPFloatingPointer = pstMPConfigurationManager->pstMPFloatingPointer;
    MemCpy( vcStringBuffer, pstMPFloatingPointer->vcSignature, 4 );
    vcStringBuffer[ 4 ] = '\0';
    Printf( "Signature : %s\n", vcStringBuffer );
    Printf( "MP Configuration Table Address : 0x%Q\n", 
            pstMPFloatingPointer->dwMPConfigurationTableAddress );
    Printf( "Length : %d * 16 Byte\n", pstMPFloatingPointer->bLength );
    Printf( "Version : %d\n", pstMPFloatingPointer->bRevision );
    Printf( "CheckSum : 0x%X\n", pstMPFloatingPointer->bCheckSum );
    Printf( "Feature Byte 1 : 0x%X ", pstMPFloatingPointer->vbMPFeatureByte[ 0 ] );
    // MP 설정 테이블 사용 여부 출력
    if( pstMPFloatingPointer->vbMPFeatureByte[ 0 ] == 0 )
    {
        Printf( "(Use MP Configuration Table)\n" );
    }
    else
    {
        Printf( "(Use Default Configuration)\n" );
    }    
    // PIC 모드 지원 여부 출력
    Printf( "Feature Byte 2 : 0x%X ", pstMPFloatingPointer->vbMPFeatureByte[ 1 ] );
    if( pstMPFloatingPointer->vbMPFeatureByte[ 2 ] & 
            MP_FLOATINGPOINTER_FEATUREBYTE2_PICMODE )
    {
        Printf( "(PIC Mode Support)\n" );
    }
    else
    {
        Printf( "(Virtual Wire Mode Support)\n" );
    }
    
    //==========================================================================
    // MP 설정 테이블 헤더 정보를 출력
    //==========================================================================
    Printf( "\n=============== MP Configuration Table Header ===============\n" );
    pstMPTableHeader = pstMPConfigurationManager->pstMPConfigurationTableHeader;
    MemCpy( vcStringBuffer, pstMPTableHeader->vcSignature, 4 );
    vcStringBuffer[ 4 ] = '\0';
    Printf( "Signature : %s\n", vcStringBuffer );
    Printf( "Length : %d Byte\n", pstMPTableHeader->wBaseTableLength );
    Printf( "Version : %d\n", pstMPTableHeader->bRevision );
    Printf( "CheckSum : 0x%X\n", pstMPTableHeader->bCheckSum );
    MemCpy( vcStringBuffer, pstMPTableHeader->vcOEMIDString, 8 );
    vcStringBuffer[ 8 ] = '\0';
    Printf( "OEM ID String : %s\n", vcStringBuffer );
    MemCpy( vcStringBuffer, pstMPTableHeader->vcProductIDString, 12 );
    vcStringBuffer[ 12 ] = '\0';
    Printf( "Product ID String : %s\n", vcStringBuffer );
    Printf( "OEM Table Pointer : 0x%X\n", 
             pstMPTableHeader->dwOEMTablePointerAddress );
    Printf( "OEM Table Size : %d Byte\n", pstMPTableHeader->wOEMTableSize );
    Printf( "Entry Count : %d\n", pstMPTableHeader->wEntryCount );
    Printf( "Memory Mapped I/O Address Of Local APIC : 0x%X\n",
            pstMPTableHeader->dwMemoryMapIOAddressOfLocalAPIC );
    Printf( "Extended Table Length : %d Byte\n", 
            pstMPTableHeader->wExtendedTableLength );
    Printf( "Extended Table Checksum : 0x%X\n", 
            pstMPTableHeader->bExtendedTableChecksum );
    
    Printf( "Press any key to continue... ('q' is exit) : " );
    if( GetCh() == 'q' )
    {
        Printf( "\n" );
        return ;
    }
    Printf( "\n" );
    
    //==========================================================================
    // 기본 MP 설정 테이블 엔트리 정보를 출력
    //==========================================================================
    Printf( "\n============= Base MP Configuration Table Entry =============\n" );
    qwBaseEntryAddress = pstMPFloatingPointer->dwMPConfigurationTableAddress + 
        sizeof( MPCONFIGURATIONTABLEHEADER );
    for( i = 0 ; i < pstMPTableHeader->wEntryCount ; i++ )
    {
        bEntryType = *( BYTE* ) qwBaseEntryAddress;
        switch( bEntryType )
        {
            // 프로세스 엔트리 정보 출력
        case MP_ENTRYTYPE_PROCESSOR:
            pstProcessorEntry = ( PROCESSORENTRY* ) qwBaseEntryAddress;
            Printf( "Entry Type : Processor\n" );
            Printf( "Local APIC ID : %d\n", pstProcessorEntry->bLocalAPICID );
            Printf( "Local APIC Version : 0x%X\n", pstProcessorEntry->bLocalAPICVersion );
            Printf( "CPU Flags : 0x%X ", pstProcessorEntry->bCPUFlags );
            // Enable/Disable 출력
            if( pstProcessorEntry->bCPUFlags & MP_PROCESSOR_CPUFLAGS_ENABLE )
            {
                Printf( "(Enable, " );
            }
            else
            {
                Printf( "(Disable, " );
            }
            // BSP/AP 출력
            if( pstProcessorEntry->bCPUFlags & MP_PROCESSOR_CPUFLAGS_BSP )
            {
                Printf( "BSP)\n" );
            }
            else
            {
                Printf( "AP)\n" );
            }            
            Printf( "CPU Signature : 0x%X\n", pstProcessorEntry->vbCPUSignature );
            Printf( "Feature Flags : 0x%X\n\n", pstProcessorEntry->dwFeatureFlags );

            // 프로세스 엔트리의 크기만큼 어드레스를 증가시켜 다음 엔트리로 이동
            qwBaseEntryAddress += sizeof( PROCESSORENTRY );
            break;

            // 버스 엔트리 정보 출력
        case MP_ENTRYTYPE_BUS:
            pstBusEntry = ( BUSENTRY* ) qwBaseEntryAddress;
            Printf( "Entry Type : Bus\n" );
            Printf( "Bus ID : %d\n", pstBusEntry->bBusID );
            MemCpy( vcStringBuffer, pstBusEntry->vcBusTypeString, 6 );
            vcStringBuffer[ 6 ] = '\0';
            Printf( "Bus Type String : %s\n\n", vcStringBuffer );
            
            // 버스 엔트리의 크기만큼 어드레스를 증가시켜 다음 엔트리로 이동
            qwBaseEntryAddress += sizeof( BUSENTRY );
            break;
            
            // I/O APIC 엔트리
        case MP_ENTRYTYPE_IOAPIC:
            pstIOAPICEntry = ( IOAPICENTRY* ) qwBaseEntryAddress;
            Printf( "Entry Type : I/O APIC\n" );
            Printf( "I/O APIC ID : %d\n", pstIOAPICEntry->bIOAPICID );
            Printf( "I/O APIC Version : 0x%X\n", pstIOAPICEntry->bIOAPICVersion );
            Printf( "I/O APIC Flags : 0x%X ", pstIOAPICEntry->bIOAPICFlags );
            // Enable/Disable 출력
            if( pstIOAPICEntry->bIOAPICFlags == 1 )
            {
                Printf( "(Enable)\n" );
            }
            else
            {
                Printf( "(Disable)\n" );
            }
            Printf( "Memory Mapped I/O Address : 0x%X\n\n", 
                    pstIOAPICEntry->dwMemoryMapAddress );

            // I/O APIC 엔트리의 크기만큼 어드레스를 증가시켜 다음 엔트리로 이동
            qwBaseEntryAddress += sizeof( IOAPICENTRY );
            break;
            
            // I/O 인터럽트 지정 엔트리
        case MP_ENTRYTYPE_IOINTERRUPTASSIGNMENT:
            pstIOAssignmentEntry = ( IOINTERRUPTASSIGNMENTENTRY* ) 
                qwBaseEntryAddress;
            Printf( "Entry Type : I/O Interrupt Assignment\n" );
            Printf( "Interrupt Type : 0x%X ", pstIOAssignmentEntry->bInterruptType );
            // 인터럽트 타입 출력
            Printf( "(%s)\n", vpcInterruptType[ pstIOAssignmentEntry->bInterruptType ] );
            Printf( "I/O Interrupt Flags : 0x%X ", pstIOAssignmentEntry->wInterruptFlags );
            // 극성과 트리거 모드 출력
            Printf( "(%s, %s)\n", vpcInterruptFlagsPO[ pstIOAssignmentEntry->wInterruptFlags & 0x03 ], 
                    vpcInterruptFlagsEL[ ( pstIOAssignmentEntry->wInterruptFlags >> 2 ) & 0x03 ] );
            Printf( "Source BUS ID : %d\n", pstIOAssignmentEntry->bSourceBUSID );
            Printf( "Source BUS IRQ : %d\n", pstIOAssignmentEntry->bSourceBUSIRQ );
            Printf( "Destination I/O APIC ID : %d\n", 
                     pstIOAssignmentEntry->bDestinationIOAPICID );
            Printf( "Destination I/O APIC INTIN : %d\n\n", 
                     pstIOAssignmentEntry->bDestinationIOAPICINTIN );

            // I/O 인터럽트 지정 엔트리의 크기만큼 어드레스를 증가시켜 다음 엔트리로 이동
            qwBaseEntryAddress += sizeof( IOINTERRUPTASSIGNMENTENTRY );
            break;
            
            // 로컬 인터럽트 지정 엔트리
        case MP_ENTRYTYPE_LOCALINTERRUPTASSIGNMENT:
            pstLocalAssignmentEntry = ( LOCALINTERRUPTASSIGNMENTENTRY* )
                qwBaseEntryAddress;
            Printf( "Entry Type : Local Interrupt Assignment\n" );
            Printf( "Interrupt Type : 0x%X ", pstLocalAssignmentEntry->bInterruptType );
            // 인터럽트 타입 출력
            Printf( "(%s)\n", vpcInterruptType[ pstLocalAssignmentEntry->bInterruptType ] );
            Printf( "I/O Interrupt Flags : 0x%X ", pstLocalAssignmentEntry->wInterruptFlags );
            // 극성과 트리거 모드 출력
            Printf( "(%s, %s)\n", vpcInterruptFlagsPO[ pstLocalAssignmentEntry->wInterruptFlags & 0x03 ], 
                    vpcInterruptFlagsEL[ ( pstLocalAssignmentEntry->wInterruptFlags >> 2 ) & 0x03 ] );
            Printf( "Source BUS ID : %d\n", pstLocalAssignmentEntry->bSourceBUSID );
            Printf( "Source BUS IRQ : %d\n", pstLocalAssignmentEntry->bSourceBUSIRQ );
            Printf( "Destination Local APIC ID : %d\n", 
                     pstLocalAssignmentEntry->bDestinationLocalAPICID );
            Printf( "Destination Local APIC LINTIN : %d\n\n", 
                     pstLocalAssignmentEntry->bDestinationLocalAPICLINTIN );
            
            // 로컬 인터럽트 지정 엔트리의 크기만큼 어드레스를 증가시켜 다음 엔트리로 이동
            qwBaseEntryAddress += sizeof( LOCALINTERRUPTASSIGNMENTENTRY );
            break;
            
        default :
            Printf( "Unknown Entry Type. %d\n", bEntryType );
            break;
        }

        // 3개를 출력하고 나면 키 입력을 대기
        if( ( i != 0 ) && ( ( ( i + 1 ) % 3 ) == 0 ) )
        {
            Printf( "Press any key to continue... ('q' is exit) : " );
            if( GetCh() == 'q' )
            {
                Printf( "\n" );
                return ;
            }
            Printf( "\n" );            
        }        
    }
}

/**
 *  ISA 버스가 연결된 I/O APIC 엔트리를 검색
 *      AnalysisMPConfigurationTable() 함수를 먼저 호출한 뒤에 사용해야 함
 */
IOAPICENTRY* FindIOAPICEntryForISA( void )
{
    MPCONFIGRUATIONMANAGER* pstMPManager;
    MPCONFIGURATIONTABLEHEADER* pstMPHeader;
    IOINTERRUPTASSIGNMENTENTRY* pstIOAssignmentEntry;
    IOAPICENTRY* pstIOAPICEntry;
    QWORD qwEntryAddress;
    BYTE bEntryType;
    BOOL bFind = 0;
    int i;
    
    // MP 설정 테이블 헤더의 시작 어드레스와 엔트리의 시작 어드레스를 저장
    pstMPHeader = gs_stMPConfigurationManager.pstMPConfigurationTableHeader;
    qwEntryAddress = gs_stMPConfigurationManager.qwBaseEntryStartAddress;
    
    //==========================================================================
    // ISA 버스와 관련된 I/O 인터럽트 지정 엔트리를 검색
    //==========================================================================
    // 모든 엔트리를 돌면서 ISA 버스와 관련된 I/O 인터럽트 지정 엔트리만 검색
    for( i = 0 ; ( i < pstMPHeader->wEntryCount ) &&
                 ( bFind == 0 ) ; i++ )
    {
        bEntryType = *( BYTE* ) qwEntryAddress;
        switch( bEntryType )
        {
            // 프로세스 엔트리는 무시
        case MP_ENTRYTYPE_PROCESSOR:
            qwEntryAddress += sizeof( PROCESSORENTRY );
            break;
            
            // 버스 엔트리, I/O APIC 엔트리, 로컬 인터럽트 지정 엔트리는 무시
        case MP_ENTRYTYPE_BUS:
        case MP_ENTRYTYPE_IOAPIC:
        case MP_ENTRYTYPE_LOCALINTERRUPTASSIGNMENT:
            qwEntryAddress += 8;
            break;
            
            // IO 인터럽트 지정 엔트리이면, ISA 버스에 관련된 엔트리인지 확인
        case MP_ENTRYTYPE_IOINTERRUPTASSIGNMENT:
            pstIOAssignmentEntry = ( IOINTERRUPTASSIGNMENTENTRY* ) qwEntryAddress;
            // MP Configuration Manager 자료구조에 저장된 ISA 버스 ID와 비교
            if( pstIOAssignmentEntry->bSourceBUSID == 
                gs_stMPConfigurationManager.bISABusID )
            {
                bFind = 1;
            }                    
            qwEntryAddress += sizeof( IOINTERRUPTASSIGNMENTENTRY );
            break;
        }
    }

    // 여기까지 왔는데 못 찾았다면 NULL을 반환
    if( bFind == FALSE )
    {
        return 0;
    }
    
    //==========================================================================
    // ISA 버스와 관련된 I/O APIC를 검색하여 I/O APIC의 엔트리를 반환
    //==========================================================================
    // 다시 엔트리를 돌면서 IO 인터럽트 지정 엔트리에 저장된 I/O APIC의 ID와 일치하는
    // 엔트리를 검색
    qwEntryAddress = gs_stMPConfigurationManager.qwBaseEntryStartAddress;
    for( i = 0 ; i < pstMPHeader->wEntryCount ; i++ )
    {
        bEntryType = *( BYTE* ) qwEntryAddress;
        switch( bEntryType )
        {
            // 프로세스 엔트리는 무시
        case MP_ENTRYTYPE_PROCESSOR:
            qwEntryAddress += sizeof( PROCESSORENTRY );
            break;
            
            // 버스 엔트리, IO 인터럽트 지정 엔트리, 로컬 인터럽트 지정 엔트리는 무시
        case MP_ENTRYTYPE_BUS:
        case MP_ENTRYTYPE_IOINTERRUPTASSIGNMENT:
        case MP_ENTRYTYPE_LOCALINTERRUPTASSIGNMENT:
            qwEntryAddress += 8;
            break;
            
            // I/O APIC 엔트리이면 ISA 버스가 연결된 엔트리인지 확인하여 반환
        case MP_ENTRYTYPE_IOAPIC:
            pstIOAPICEntry = ( IOAPICENTRY* ) qwEntryAddress;
            if( pstIOAPICEntry->bIOAPICID == pstIOAssignmentEntry->bDestinationIOAPICID )
            {
                return pstIOAPICEntry;
            }
            qwEntryAddress += sizeof( IOINTERRUPTASSIGNMENTENTRY );
            break;
        }
    }
    
    return 0;
}

/**
 *  프로세서 또는 코어의 개수를 반환
 */
int GetProcessorCount( void )
{
    // MP 설정 테이블이 없을 수도 있으므로, 0으로 설정된 경우 1을 반환
    if( gs_stMPConfigurationManager.iProcessorCount == 0 )
    {
        return 1;
    }
    return gs_stMPConfigurationManager.iProcessorCount;
}
