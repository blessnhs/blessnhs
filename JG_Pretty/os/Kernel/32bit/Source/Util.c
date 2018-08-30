/*
 * Util.c
 *
 *  Created on: 2017. 9. 16.
 *      Author: user
 */
#include "Util.h"

void WriteLine( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;

    pstScreen += ( iY * 80 ) + iX;

    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {

        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

/**
 *  IA-32e 모드용 커널 영역을 0으로 초기화
 *      1Mbyte ~ 6Mbyte까지 영역을 초기화
 */
BOOL Initialize64KernelMemory( void )
{
    DWORD* pdwCurrentAddress;

    // 초기화를 시작할 어드레스인 0x100000(1MB)을 설정
    pdwCurrentAddress = ( DWORD* ) 0x100000;

    // 마지막 어드레스인 0x600000(6MB)까지 루프를 돌면서 4바이트씩 0으로 채움
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {
        *pdwCurrentAddress = 0x00;

        // 0으로 저장한 후 다시 읽었을 때 0이 나오지 않으면 해당 어드레스를
        // 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }

        // 다음 어드레스로 이동
        pdwCurrentAddress++;
    }

    return TRUE;
}

/**
 *  JG OS를 실행하기에 충분한 메모리를 가지고 있는지 체크
 *      64Mbyte 이상의 메모리를 가지고 있는지 검사
 */
BOOL CheckSystemMemory( void )
{
   DWORD* pdwCurrentAddress;

    // 0x100000(1MB)부터 검사 시작
   pdwCurrentAddress = ( DWORD* ) 0x100000;

    // 0x4000000(64MB)까지 루프를 돌면서 확인
   while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
   {
       *pdwCurrentAddress = 0x12345678;

        // 0x12345678로 저장한 후 다시 읽었을 때 0x12345678이 나오지 않으면
        // 해당 어드레스를 사용하는데 문제가 생긴 것이므로 더이상 진행하지 않고 종료
       if( *pdwCurrentAddress != 0x12345678 )
       {
           return FALSE;
       }

        // 1MB씩 이동하면서 확인
       pdwCurrentAddress += ( 0x100000 / 4 );
   }
   return TRUE;
}

/**
 *  IA-32e 모드 커널을 0x200000(2Mbyte) 어드레스에 복사
 */
void CopyKernel64ImageTo2Mbyte( void )
{
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD* pdwSourceAddress,* pdwDestinationAddress;
    int i;

    // 0x7C05에 총 커널 섹터 수, 0x7C07에 보호 모드 커널 섹터 수가 들어 있음
    wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
    wKernel32SectorCount = *( ( WORD* ) 0x7C07 );

    pdwSourceAddress = ( DWORD* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
    pdwDestinationAddress = ( DWORD* ) 0x200000;
    // IA-32e 모드 커널 섹터 크기만큼 복사
    for( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4;
        i++ )
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}




