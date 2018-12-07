/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS에서 사용할 유틸리티 함수에 관련된 파일
 */

#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>
#include "VBE.h"
#include "../../32bit/source/page.h"

// PIT 컨트롤러가 발생한 횟수를 저장할 카운터
volatile QWORD g_qwTickCount = 0;

/**
 *  메모리를 특정 값으로 채움
 */


void *MemChr(const void *buf, int c, int n)
{
    unsigned char *p = (unsigned char *)buf;
    unsigned char *end = p + n;

    while (p != end)
    {
        if (*p == c)
        {
            return p;
        }

        ++p;
    }

    return 0;
}

void MemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;
    QWORD qwData;
    int iRemainByteStartOffset;
    
    // 8 바이트 데이터를 채움
    qwData = 0;
    for( i = 0 ; i < 8 ; i++ )
    {
        qwData = ( qwData << 8 ) | bData;
    }
    
    // 8 바이트씩 먼저 채움
    for( i = 0 ; i < ( iSize / 8 ) ; i++ )
    {
        ( ( QWORD* ) pvDestination )[ i ] = qwData;
    }
    
    // 8 바이트씩 채우고 남은 부분을 마무리
    iRemainByteStartOffset = i * 8;
    for( i = 0 ; i < ( iSize % 8 ) ; i++ )
    {
        ( ( char* ) pvDestination )[ iRemainByteStartOffset++ ] = bData;
    }
}

/**
 *  메모리를 16bit 특정 값으로 채움
 *      iSize는 채울 16bit 데이터의 개수를 의미
 */
inline void MemSetWord( void* pvDestination, WORD wData, int iWordSize )
{
    int i;
    QWORD qwData;
    int iRemainWordStartOffset;
    
    // 8 바이트에 WORD 데이터를 채움
    qwData = 0;
    for( i = 0 ; i < 4 ; i++ )
    {
        qwData = ( qwData << 16 ) | wData;
    }
    
    // 8 바이트씩 먼저 채움, WORD 데이터를 4개씩 한꺼번에 채울 수 있음
    for( i = 0 ; i < ( iWordSize / 4 ) ; i++ )
    {
        ( ( QWORD* ) pvDestination )[ i ] = qwData;
    }
    
    // 8 바이트씩 채우고 남은 부분을 마무리
    iRemainWordStartOffset = i * 4;
    for( i = 0 ; i < ( iWordSize % 4 ) ; i++ )
    {
        ( ( WORD* ) pvDestination )[ iRemainWordStartOffset++ ] = wData;
    }
}


/*
void MemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;
    
    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = bData;
    }
}
*/

/**
 *  메모리 복사
 */
int MemCpy( void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    int iRemainByteStartOffset;
    
    // 8 바이트씩 먼저 복사
    for( i = 0 ; i < ( iSize / 8 ) ; i++ )
    {
        ( ( QWORD* ) pvDestination )[ i ] = ( ( QWORD* ) pvSource )[ i ];
    }
    
    // 8 바이트씩 채우고 남은 부분을 마무리
    iRemainByteStartOffset = i * 8;
    for( i = 0 ; i < ( iSize % 8 ) ; i++ )
    {
        ( ( char* ) pvDestination )[ iRemainByteStartOffset ] = 
            ( ( char* ) pvSource )[ iRemainByteStartOffset ];
        iRemainByteStartOffset++;
    }
    return iSize;
}

int32_t strcmp(const char* s1, const char* s2)
{
    while ((*s1) && (*s1 == *s2))
    {
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

int strncmp (const char *cs, const char *ct, size_t count)
{
	signed char res = 0;

	while (count) {
		if ((res = *cs - *ct ++) || !*cs ++)
			break;

		count --;
	}

	return res;
}

int memcpy( void* pvDestination, const void* pvSource, int iSize )
{
	MemCpy(pvDestination,pvSource,iSize);
}

/*
int MemCpy( void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    
    for( i = 0 ; i < iSize ; i++ )
    {
        ( ( char* ) pvDestination )[ i ] = ( ( char* ) pvSource )[ i ];
    }
    
    return iSize;
}
*/

/**
 *  메모리 비교
 */
int MemCmp( const void* pvDestination, const void* pvSource, int iSize )
{
    int i, j;
    int iRemainByteStartOffset;
    QWORD qwValue;
    char cValue;
    
    // 8 바이트씩 먼저 비교
    for( i = 0 ; i < ( iSize / 8 ) ; i++ )
    {
        qwValue = ( ( QWORD* ) pvDestination )[ i ] - ( ( QWORD* ) pvSource )[ i ];

        // 틀린 위치를 정확하게 찾아서 그 값을 반환
        if( qwValue != 0 )
        {
            for( i = 0 ; i < 8 ; i++ )
            {
                if( ( ( qwValue >> ( i * 8 ) ) & 0xFF ) != 0 )
                {
                    return ( qwValue >> ( i * 8 ) ) & 0xFF;
                }
            }
        }
    }
    
    // 8 바이트씩 채우고 남은 부분을 마무리
    iRemainByteStartOffset = i * 8;
    for( i = 0 ; i < ( iSize % 8 ) ; i++ )
    {
        cValue = ( ( char* ) pvDestination )[ iRemainByteStartOffset ] -
            ( ( char* ) pvSource )[ iRemainByteStartOffset ];
        if( cValue != 0 )
        {
            return cValue;
        }
        iRemainByteStartOffset++;
    }    
    return 0;
}
/*
int MemCmp( const void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    char cTemp;
    
    for( i = 0 ; i < iSize ; i++ )
    {
        cTemp = ( ( char* ) pvDestination )[ i ] - ( ( char* ) pvSource )[ i ];
        if( cTemp != 0 )
        {
            return ( int ) cTemp;
        }
    }
    return 0;
}
*/

/**
 *  RFLAGS 레지스터의 인터럽트 플래그를 변경하고 이전 인터럽트 플래그의 상태를 반환
 */
BOOL SetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;
    
    // 이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
    qwRFLAGS = ReadRFLAGS();
    if( bEnableInterrupt == TRUE )
    {
        EnableInterrupt();
    }
    else
    {
        DisableInterrupt();
    }
    
    // 이전 RFLAGS 레지스터의 IF 비트(비트 9)를 확인하여 이전의 인터럽트 상태를 반환
    if( qwRFLAGS & 0x0200 )
    {
        return TRUE;
    }
    return FALSE;
}

/**
 *  문자열의 길이를 반환
 */
int kStrLen( const char* pcBuffer )
{
    int i;
    
    for( i = 0 ; ; i++ )
    {
        if( pcBuffer[ i ] == '\0' )
        {
            break;
        }
    }
    return i;
}

// 램의 총 크기(Mbyte 단위)
static gs_qwTotalRAMMBSize = 0;

/**
 *  64Mbyte 이상의 위치부터 램 크기를 체크
 *      최초 부팅 과정에서 한번만 호출해야 함
 */
void CheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    // 64Mbyte(0x4000000)부터 4Mbyte단위로 검사 시작
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while( 1 )
    {
        // 이전에 메모리에 있던 값을 저장
        dwPreviousValue = *pdwCurrentAddress;
        // 0x12345678을 써서 읽었을 때 문제가 없는 곳까지를 유효한 메모리 
        // 영역으로 인정
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
        {
            break;
        }
        // 이전 메모리 값으로 복원
        *pdwCurrentAddress = dwPreviousValue;
        // 다음 4Mbyte 위치로 이동
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    // 체크가 성공한 어드레스를 1Mbyte로 나누어 Mbyte 단위로 계산
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}   

/**
 *  RAM 크기를 반환
 */
QWORD GetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  atoi() 함수의 내부 구현
 */
long AToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch( iRadix )
    {
        // 16진수
    case 16:
        lReturn = HexStringToQword( pcBuffer );
        break;
        
        // 10진수 또는 기타
    case 10:
    default:
        lReturn = DecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  16진수 문자열을 QWORD로 변환 
 */
QWORD HexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    // 문자열을 돌면서 차례로 변환
    for( i = 0 ; pcBuffer[ i ] != '\0' ; i++ )
    {
        qwValue *= 16;
        if( ( 'A' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'Z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'A' ) + 10;
        }
        else if( ( 'a' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'z' ) )
        {
            qwValue += ( pcBuffer[ i ] - 'a' ) + 10;
        }
        else 
        {
            qwValue += pcBuffer[ i ] - '0';
        }
    }
    return qwValue;
}

/**
 *  10진수 문자열을 long으로 변환
 */
long DecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    // 음수이면 -를 제외하고 나머지를 먼저 long으로 변환
    if( pcBuffer[ 0 ] == '-' )
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    
    // 문자열을 돌면서 차례로 변환
    for( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    // 음수이면 - 추가
    if( pcBuffer[ 0 ] == '-' )
    {
        lValue = -lValue;
    }
    return lValue;
}

/**
 *  itoa() 함수의 내부 구현
 */
int IToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch( iRadix )
    {
        // 16진수
    case 16:
        iReturn = HexToString( lValue, pcBuffer );
        break;
        
        // 10진수 또는 기타
    case 10:
    default:
        iReturn = DecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  16진수 값을 문자열로 변환
 */
int HexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    // 0이 들어오면 바로 처리
    if( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // 버퍼에 1의 자리부터 16, 256, ...의 자리 순서로 숫자 삽입
    for( i = 0 ; qwValue > 0 ; i++ )
    {
        qwCurrentValue = qwValue % 16;
        if( qwCurrentValue >= 10 )
        {
            pcBuffer[ i ] = 'A' + ( qwCurrentValue - 10 );
        }
        else
        {
            pcBuffer[ i ] = '0' + qwCurrentValue;
        }
        
        qwValue = qwValue / 16;
    }
    pcBuffer[ i ] = '\0';
    
    // 버퍼에 들어있는 문자열을 뒤집어서 ... 256, 16, 1의 자리 순서로 변경
    ReverseString( pcBuffer );
    return i;
}

/**
 *  10진수 값을 문자열로 변환
 */
int DecimalToString( long lValue, char* pcBuffer )
{
    long i;

    // 0이 들어오면 바로 처리
    if( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    // 만약 음수이면 출력 버퍼에 '-'를 추가하고 양수로 변환
    if( lValue < 0 )
    {
        i = 1;
        pcBuffer[ 0 ] = '-';
        lValue = -lValue;
    }
    else
    {
        i = 0;
    }

    // 버퍼에 1의 자리부터 10, 100, 1000 ...의 자리 순서로 숫자 삽입
    for( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    // 버퍼에 들어있는 문자열을 뒤집어서 ... 1000, 100, 10, 1의 자리 순서로 변경
    if( pcBuffer[ 0 ] == '-' )
    {
        // 음수인 경우는 부호를 제외하고 문자열을 뒤집음
        ReverseString( &( pcBuffer[ 1 ] ) );
    }
    else
    {
        ReverseString( pcBuffer );
    }
    
    return i;
}

/**
 *  문자열의 순서를 뒤집음
 */
void ReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   
   // 문자열의 가운데를 중심으로 좌/우를 바꿔서 순서를 뒤집음
   iLength = kStrLen( pcBuffer );
   for( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  sPrintf() 함수의 내부 구현
 */
int SPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    // 가변 인자를 꺼내서 vsPrintf() 함수에 넘겨줌
    va_start( ap, pcFormatString );
    iReturn = VSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  vsPrintf() 함수의 내부 구현
 *      버퍼에 포맷 문자열에 따라 데이터를 복사
 */
int VSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j, k;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    double dValue;
    
    // 포맷 문자열의 길이를 읽어서 문자열의 길이만큼 데이터를 출력 버퍼에 출력
    iFormatLength = kStrLen( pcFormatString );
    for( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // %로 시작하면 데이터 타입 문자로 처리
        if( pcFormatString[ i ] == '%' ) 
        {
            // % 다음의 문자로 이동
            i++;
            switch( pcFormatString[ i ] ) 
            {
                // 문자열 출력  
            case 's':
                // 가변 인자에 들어있는 파라미터를 문자열 타입으로 변환
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                // 문자열의 길이만큼을 출력 버퍼로 복사하고 출력한 길이만큼 
                // 버퍼의 인덱스를 이동
                MemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
                
                // 문자 출력
            case 'c':
                // 가변 인자에 들어있는 파라미터를 문자 타입으로 변환하여 
                // 출력 버퍼에 복사하고 버퍼의 인덱스를 1만큼 이동
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

                // 정수 출력
            case 'u':
            case 'd':
            case 'i':
                // 가변 인자에 들어있는 파라미터를 정수 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += IToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
                // 4바이트 Hex 출력
            case 'x':
            case 'X':
                // 가변 인자에 들어있는 파라미터를 DWORD 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += IToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

                // 8바이트 Hex 출력
            case 'q':
            case 'Q':
            case 'p':
                // 가변 인자에 들어있는 파라미터를 QWORD 타입으로 변환하여
                // 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += IToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;
            
                // 소수점 둘째 자리까지 실수를 출력
            case 'f':
                dValue = ( double) ( va_arg( ap, double ) );
                // 셋째 자리에서 반올림 처리
                dValue += 0.005;
                // 소수점 둘째 자리부터 차례로 저장하여 버퍼를 뒤집음
                pcBuffer[ iBufferIndex ] = '0' + ( QWORD ) ( dValue * 100 ) % 10;
                pcBuffer[ iBufferIndex + 1 ] = '0' + ( QWORD ) ( dValue * 10 ) % 10;
                pcBuffer[ iBufferIndex + 2 ] = '.';
                for( k = 0 ; ; k++ )
                {
                    // 정수 부분이 0이면 종료
                    if( ( ( QWORD ) dValue == 0 ) && ( k != 0 ) )
                    {
                        break;
                    }
                    pcBuffer[ iBufferIndex + 3 + k ] = '0' + ( ( QWORD ) dValue % 10 );
                    dValue = dValue / 10;
                }
                pcBuffer[ iBufferIndex + 3 + k ] = '\0';
                // 값이 저장된 길이만큼 뒤집고 길이를 증가시킴
                ReverseString( pcBuffer + iBufferIndex );
                iBufferIndex += 3 + k;
                break;
                // 위에 해당하지 않으면 문자를 그대로 출력하고 버퍼의 인덱스를
                // 1만큼 이동
            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // 일반 문자열 처리
        else
        {
            // 문자를 그대로 출력하고 버퍼의 인덱스를 1만큼 이동
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // NULL을 추가하여 완전한 문자열로 만들고 출력한 문자의 길이를 반환
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  Tick Count를 반환
 */
QWORD GetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  밀리세컨드(milisecond) 동안 대기
 */
void Sleep( QWORD qwMillisecond )
{
    QWORD qwLastTickCount;
    
    qwLastTickCount = g_qwTickCount;
    
    while( ( g_qwTickCount - qwLastTickCount ) <= qwMillisecond )
    {
        Schedule();
    }
}

/**
 *  그래픽 모드인지 여부를 반환
 */
BOOL IsGraphicMode( void )
{
    // 그래픽 모드 시작 여부가 저장된 어드레스(0x7C0A)
    if( *( BYTE* ) VBE_STARTGRAPHICMODEFLAGADDRESS == 0 )
    {
        return FALSE;
    }
    
    return TRUE;
}


unsigned long long *GetEntryLevel(unsigned long long address, int level)
{
	unsigned long long index = address / 0x1000;
	unsigned long long indices[4] = {
        index % 512,
        (index >> 9) % 512,
        (index >> 18) % 512,
        (index >> 27) % 512
    };
	unsigned long long *latest = (unsigned long long *)ReadCr3();

	int l;
    for(l = 3; l >= level; l --) {
    	unsigned long long *followed = (unsigned long long *)(*latest & ~0xfff);
        // if the present bit (P) is clear and we have more to go, then error.
        if(l != level && (*latest & 1) == 0) {
            return 0;
        }
        // TODO: check for size bit here
        latest = followed + indices[l];
    }
    return *latest;
}

unsigned long long *GetPhysAddr(void *virtualaddr)
{
	unsigned long pml4index     = (unsigned long)virtualaddr >> 39 & 0x1ff;
	unsigned long pdpindex 		= (unsigned long)virtualaddr >> 30 & 0x1ff;
	unsigned long pdindex  		= (unsigned long)virtualaddr >> 21 & 0x1ff;
//	unsigned long ptindex  		= (unsigned long)virtualaddr >> 12 & 0x1ff;
//	unsigned long offset   		= (unsigned long)virtualaddr & 0xfff;

	//4단계까지만 사용 (pml4,pdp,pd)

//	Printf("pml4 %d pdpindex %d pdindex %d \n",	pml4index,pdpindex,pdindex);

	PML4TENTRY *pstPML4TEntry = ( PML4TENTRY* ) ReadCr3();
//	PDPTENTRY  *pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
//	PDENTRY    *pstPDEntry = ( PDENTRY* ) 0x102000;

	unsigned long lowAddress = ((pstPML4TEntry[pml4index].dwAttributeAndLowerBaseAddress >> 12));
	unsigned long upperAddress = (pstPML4TEntry[pml4index].dwUpperBaseAddressAndEXB & 0xff);

	unsigned long long pdp4Address = lowAddress;
	pdp4Address = pdp4Address << 12 | upperAddress;
//	Printf("lowAddress %q upperAddress %q pdpe Address %q base %q\n",lowAddress,upperAddress,pdp4Address,pstPML4TEntry->dwAttributeAndLowerBaseAddress);

	PDPTENTRY *pstPDPTEntry = ( PDPTENTRY* ) pdp4Address;

	lowAddress = ((pstPDPTEntry[pdpindex].dwAttributeAndLowerBaseAddress >> 12));
	upperAddress = (pstPDPTEntry[pdpindex].dwUpperBaseAddressAndEXB & 0xff);
	unsigned long long pdAddress = lowAddress;
	pdAddress = pdAddress << 12 | upperAddress;

//	Printf("lowAddress %q upperAddress %q pde Address %q base %q\n",lowAddress,upperAddress,pdAddress,pstPDPTEntry->dwAttributeAndLowerBaseAddress);
	PDENTRY    *pstPDEntry = ( PDENTRY* ) pdAddress;

	lowAddress = ((pstPDEntry[pdindex].dwAttributeAndLowerBaseAddress >> 12));
	upperAddress = (pstPDEntry[pdindex].dwUpperBaseAddressAndEXB & 0xff);
	unsigned long long pAddress = lowAddress;
	pdAddress = pdAddress << 12 | upperAddress;


//	Printf("pdentry %q %q\n",virtualaddr,pdAddress);

	unsigned long virtualoffset = ((unsigned long )virtualaddr & 0x1FFFFF);

	unsigned long long *ptr = (unsigned long long *)pdAddress + virtualoffset;

	return ptr;//(unsigned long long *)(&pstPDEntry[pdindex] + ((unsigned long long *)virtualaddr & (unsigned long long *)0x1FFFFF));
}

size_t strlen(const char* str)
{
    size_t retval = 0;
    for (; *str != '\0'; ++str)
        ++retval;
    return retval;
}

int bsr(int val)
{
  int result;
  __asm__("bsr %1, %0" : "=r"(result) : "r"(val));
  return result;
}


uint32_t alignUp(uint32_t val, uint32_t alignment)
{
    if (!alignment)
        return val;
    --alignment;
    return (val+alignment) & ~alignment;
}


uint32_t alignDown(uint32_t val, uint32_t alignment)
{
    if (!alignment)
        return val;
    return val & ~(alignment-1);
}

uint32_t* MemSetl(uint32_t* dest, uint32_t val, size_t dwords)
{
    uint32_t* retval = dest;
    __asm__ volatile("cld\n"
                     "rep stosl" : "+D"(dest), "+c"(dwords) : "a"(val) : "memory");

    return retval;
}

void* memset(void* dest, char val, size_t bytes)
{
    void* retval = dest;
    uint8_t uval = val;
    size_t dwords = bytes/4; // Number of dwords (4 Byte blocks) to be written
    bytes %= 4;              // Remaining bytes
    uint32_t dval = (uval<<24)|(uval<<16)|(uval<<8)|uval; // Create dword from byte value
    __asm__ volatile("cld\n"
                     "rep stosl\n"
                     "mov %%edx, %%ecx\n"
                     "rep stosb" : "+D"(dest), "+c"(dwords) : "a"(dval), "d"(bytes) : "memory");
    return retval;
}

uint16_t htons(uint16_t v)
{
    __asm__("xchg %h0, %b0" : "+Q"(v));
    return (v);
}

uint32_t htonl(uint32_t v)
{
    __asm__("bswap %0" : "+r"(v));
    return (v);
}

char* strcat(char* dest, const char* src)
{
    strcpy(dest + strlen(dest), src);
    return dest;
}

char* strcpy(char* dest, const char* src)
{
    char* save = dest;
    while ((*dest++ = *src++));
    return save;
}

char* strchr(const char* str, int character)
{
    for (;; str++)
    {
        // the order here is important
        if (*str == character)
        {
            return (char*)str;
        }
        if (*str == 0) // end of string
        {
            return (0);
        }
    }
}

int isspace(int c)
{
    return ((c >= 0x09 && c <= 0x0D) || c == ' ');
}


long int strtol(const char* nptr, char** endptr, int base)
{
    long num = 0;

    if (base && (base < 2 || base > 36))
        return num;

    while (isspace(*nptr))
        ++nptr; // skip spaces

    int sign = *nptr == '-' ? -1 : 1;
    if (*nptr == '-' || *nptr == '+')
        ++nptr;

    if (base == 0)
    {
        base = 10;
        if (*nptr == '0' && (*(++nptr) == 'X' || *nptr == 'x'))
        {
            ++nptr;
            base = 16;
        }
    }
    else if (base == 16)
    {
        if (*nptr == '0' && (*(++nptr) == 'X' || *nptr == 'x'))
            ++nptr;
    }

    while (*nptr == '0')
        ++nptr;

    for (; *nptr != '\0'; ++nptr)
    {
        if (*nptr - '0' < base && *nptr - '0' >= 0)
        {
            num = num * base + *nptr - '0';
            if (num <= 0)
            {
                num = -1;
                break;
            }
        }
        else if ((*nptr | 0x20) - 'a' + 10 < base &&
            (*nptr | 0x20) - 'a' + 10 >= 0)
        { // 'x' | 0x20 =~= tolower('X')
            num = num * base + (*nptr | 0x20) - 'a' + 10;
            if (num <= 0)
            {
                num = -1;
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (; *nptr != '\0'; ++nptr)
    {
        // skip rest of integer constant
        if (!(*nptr - '0' < base && *nptr - '0' >= 0) &&
                !((*nptr | 0x20) - 'a' + 10 < base) &&
                !((*nptr | 0x20) - 'a' + 10 >= 0))
        {
            break;
        }
    }

    if (endptr)
        *endptr = (char*)nptr;

    return num * sign;
}

int atoi(const char* nptr)
{
    return (int)strtol(nptr, 0, 10);
}


void memshow(const void* start, size_t count, bool alpha)
{
	size_t i = 0;
    for (i = 0; i < count; i++)
    {
        if (alpha)
        {
            Printf("%c",((const char*)start)[i]);
        }
        else
        {
            if (i%16 == 0)
            	 Printf("\n");
            Printf("%d ", ((const uint8_t*)start)[i]);
        }
    }
}

char *strstr(char *string, char *substring)
{
    char *a, *b;
    int idx = 0;
    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0)
    {
    	return string;
    }

    for ( ; *string != 0; string += 1)
    {
    	if (*string != *b)
    	{
    		continue;
    	}

    	a = string;

		while (1)
		{
			if (*b == 0)
			{
				return string;
			}
			if (*a++ != *b++)
			{
				break;
			}
		}

		b = substring;
    }
    return (char *) 0;
}

