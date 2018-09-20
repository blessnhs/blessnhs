/**
 *  file    SystemCall.c
 *  date    2009/12/08
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   시스템 콜에 관련된 함수를 정의한 소스 파일
 */

#include "Types.h"
#include "SystemCall.h"
#include "AssemblyUtility.h"
#include "Descriptor.h"
#include "SystemCallList.h"
#include "DynamicMemory.h"
#include "RTC.h"
#include "Task.h"
#include "Utility.h"
#include "Window.h"
#include "JPEG.h"
#include "Loader.h"
#include "Fat32/fat_filelib.h"
/**
 *  시스템 콜(syscall/sysret)를 호출할 수 있도록 초기화
 */
void InitializeSystemCall( void )
{
    QWORD qwRDX;
    QWORD qwRAX;
    
    // IA32_STAR MSR(0xC0000081) 설정
    // 상위 32비트는 SYSRET CS/SS[63:48]와 SYSCALL CS/SS[47:32]로 구성
    qwRDX = ( ( GDT_KERNELDATASEGMENT | SELECTOR_RPL_3 ) << 16 ) | GDT_KERNELCODESEGMENT;
    // 하위 32비트는 예약됨(Reserved)
    qwRAX = 0;
    WriteMSR( 0xC0000081, qwRDX, qwRAX );
    
    // IA32_LSTAR MSR 설정(0xC0000082), SystemCallEntryPoint() 함수의 어드레스로 지정
    WriteMSR( 0xC0000082, 0, ( QWORD ) SystemCallEntryPoint );
    
    // IA32_FMASK MSR 설정(0xC0000084), 
    // RFLAGS 레지스터의 값을 변경할 것은 없으므로 0x00으로 설정하여 값을 유지
    WriteMSR( 0xC0000084, 0, 0x00 );
}

/**
 *  시스템 콜 번호와 파라미터 자료구조를 이용해서 시스템 콜을 처리
 */ 
QWORD ProcessSystemCall( QWORD qwServiceNumber, PARAMETERTABLE* pstParameter )
{
    TCB* pstTCB;
    
    switch( qwServiceNumber )
    {
        //----------------------------------------------------------------------
        // 콘솔 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_PrintfSTRING:
        return PrintfString( PARAM( 0 ) );        
    case SYSCALL_SETCURSOR:
        SetCursor( PARAM( 0 ), PARAM( 1 ) );
        return TRUE;
    case SYSCALL_GETCURSOR:
        GetCursor( ( int* ) PARAM( 0 ), ( int* ) PARAM( 1 ) );
        return TRUE;
    case SYSCALL_CLEARSCREEN:
        ClearScreen();
        return TRUE;
    case SYSCALL_GETCH:
        return GetCh();

        // 동적 메모리 할당/해제 관련
    case SYSCALL_MALLOC:
        return ( QWORD ) AllocateMemory( PARAM( 0 ) );
    case SYSCALL_FREE:
        return FreeMemory( ( void* ) PARAM( 0 ) );

        //----------------------------------------------------------------------
        // 파일과 디렉터리 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_FOPEN:
        return ( QWORD ) fl_fopen( ( char* ) PARAM( 0 ), ( char* ) PARAM( 1 ) );
    case SYSCALL_FREAD:
        return fl_fread( ( void* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ),
                      ( FL_FILE* ) PARAM( 3 ) );
    case SYSCALL_FWRITE:
        return fl_fwrite( ( void* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ),
                       ( FL_FILE* ) PARAM( 3 ) );
    case SYSCALL_FSEEK:
        return fl_fseek( ( FL_FILE* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_FCLOSE:
        fl_fclose( ( FL_FILE* ) PARAM( 0 ) );
        return 0;
    case SYSCALL_REMOVE:
        return fl_remove( ( char* ) PARAM( 0 ) );



//    case SYSCALL_OPENDIR:
//        return ( QWORD ) opendir( ( char* ) PARAM( 0 ) );
    case SYSCALL_READDIR:
        return ( QWORD ) fl_listdirectory(( char* ) PARAM( 0 ));

    case SYSCALL_DIR_IN_FILECOUNT:
    	return ( int ) fl_directoryfilecount(( char* ) PARAM( 0 ));
//    case SYSCALL_REWINDDIR:
//        rewinddir( ( DIR* ) PARAM( 0 ) );
//        return TRUE;
//    case SYSCALL_CLOSEDIR:
//        return closedir( ( DIR* ) PARAM( 0 ) );
//    case SYSCALL_ISFILEOPENED:
//        return IsFileOpened( ( DIRECTORYENTRY* ) PARAM( 0 ) );

        //----------------------------------------------------------------------
        // 하드 디스크 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_READHDDSECTOR:
        return ReadHDDSector( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                               ( char* ) PARAM( 4 ) );
    case SYSCALL_WRITEHDDSECTOR:
        return WriteHDDSector( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                               ( char* ) PARAM( 4 ) );

        //----------------------------------------------------------------------
        // 태스크와 스케줄러 관련
        //----------------------------------------------------------------------
    case SYSCALL_CREATETASK:
        pstTCB = CreateTask( PARAM( 0 ), ( void* ) PARAM( 1 ), PARAM( 2 ), 
                              PARAM( 3 ), PARAM( 4 ) );
        if( pstTCB == NULL )
        {
            return TASK_INVALIDID;
        }   
        return pstTCB->stLink.qwID;
    case SYSCALL_SCHEDULE:
        return Schedule();        
    case SYSCALL_CHANGEPRIORITY:
        return ChangePriority( PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_ENDTASK:
        return EndTask( PARAM( 0 ) );
    case SYSCALL_EXITTASK:
        ExitTask();
        return TRUE;
    case SYSCALL_GETTASKCOUNT:
        return GetTaskCount( PARAM( 0 ) );
    case SYSCALL_ISTASKEXIST:
        return IsTaskExist( PARAM( 0 ) );
    case SYSCALL_GETPROCESSORLOAD:
        return GetProcessorLoad( PARAM( 0 ) );
    case SYSCALL_CHANGEPROCESSORAFFINITY:
        return ChangePriority( PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_EXECUTEPROGRAM:
        return ExecuteProgram( ( char* ) PARAM( 0 ), ( char* ) PARAM( 1 ), 
                                ( BYTE ) PARAM( 2 ) );
    case SYSCALL_CREATETHREAD:
        return CreateThread( PARAM( 0 ), PARAM( 1 ), ( BYTE ) PARAM( 2 ), 
                              PARAM( 3 ) );
    
        //----------------------------------------------------------------------
        // GUI 시스템 관련
        //----------------------------------------------------------------------
    case SYSCALL_GETBACKGROUNDWINDOWID:
        return GetBackgroundWindowID();
    case SYSCALL_GETSCREENAREA:
        GetScreenArea( ( RECT* ) PARAM( 0 ) );
        return TRUE;
    case SYSCALL_CREATEWINDOW:
        return CreateWindow( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                              PARAM( 4 ), ( char* ) PARAM( 5 ) );
    case SYSCALL_DELETEWINDOW:
        return DeleteWindow( PARAM( 0 ) );
    case SYSCALL_SHOWWINDOW:
        return ShowWindow( PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_FINDWINDOWBYPOINT:
        return FindWindowByPoint( PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_FINDWINDOWBYTITLE:
        return FindWindowByTitle( ( char* ) PARAM( 0 ) );
    case SYSCALL_ISWINDOWEXIST:
        return IsWindowExist( PARAM( 0 ) );
    case SYSCALL_GETTOPWINDOWID:
        return GetTopWindowID();
    case SYSCALL_MOVEWINDOWTOTOP:
        return MoveWindowToTop( PARAM( 0 ) );
    case SYSCALL_ISINTITLEBAR:
        return IsInTitleBar( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_ISINCLOSEBUTTON:
        return IsInCloseButton( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_MOVEWINDOW:
        return MoveWindow( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_RESIZEWINDOW:
        return ResizeWindow( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ),
                              PARAM( 4 ) );
    case SYSCALL_GETWINDOWAREA:
        return GetWindowArea( PARAM( 0 ), ( RECT* ) PARAM( 1 ) );
    case SYSCALL_UPDATESCREENBYID:
        return UpdateScreenByID( PARAM( 0 ) );
    case SYSCALL_UPDATESCREENBYWINDOWAREA:
        return UpdateScreenByWindowArea( PARAM( 0 ), ( RECT* ) PARAM( 1 ) );
    case SYSCALL_UPDATESCREENBYSCREENAREA:
        return UpdateScreenByScreenArea( ( RECT* ) PARAM( 0 ) );
    case SYSCALL_SENDEVENTTOWINDOW:
        return SendEventToWindow( PARAM( 0 ), ( EVENT* ) PARAM( 1 ) );
    case SYSCALL_RECEIVEEVENTFROMWINDOWQUEUE:
        return ReceiveEventFromWindowQueue( PARAM( 0 ), ( EVENT* ) PARAM( 1 ) );
    case SYSCALL_DRAWWINDOWFRAME:
        return DrawWindowFrame( PARAM( 0 ) );
    case SYSCALL_DRAWWINDOWBACKGROUND:
        return DrawWindowBackground( PARAM( 0 ) );
    case SYSCALL_DRAWWINDOWTITLE:
        return DrawWindowTitle( PARAM( 0 ), ( char* ) PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_DRAWBUTTON:
        return DrawButton( PARAM( 0 ), ( RECT* ) PARAM( 1 ), PARAM( 2 ), 
                            ( char* ) PARAM( 3 ), PARAM( 4 ) );
    case SYSCALL_DRAWPIXEL:
        return DrawPixel( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ) );
    case SYSCALL_DRAWLINE:
        return DrawLine( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                          PARAM( 4 ), PARAM( 5 ) );
    case SYSCALL_DRAWRECT:
        return DrawRect( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                          PARAM( 4 ), PARAM( 5 ), PARAM( 6 ) );
    case SYSCALL_DRAWCIRCLE:
        return DrawCircle( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                            PARAM( 4 ), PARAM( 5 ) );
    case SYSCALL_DRAWTEXT:
        return DrawText( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), PARAM( 3 ), 
                          PARAM( 4 ), ( char* ) PARAM( 5 ), PARAM( 6 ) );
    case SYSCALL_MOVECURSOR:
        MoveCursor( PARAM( 0 ), PARAM( 1 ) );
        return TRUE;
    case SYSCALL_GETCURSORPOSITION:
        GetCursorPosition( ( int* ) PARAM( 0 ), ( int* ) PARAM( 1 ) );
        return TRUE;
    case SYSCALL_BITBLT:
        return BitBlt( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), ( COLOR* ) PARAM( 3 ), 
                        PARAM( 4 ), PARAM( 5 ) );

        //----------------------------------------------------------------------
        // JPEG 관련
        //----------------------------------------------------------------------
    case SYSCALL_JPEGINIT:
        return JPEGInit( ( JPEG* ) PARAM( 0 ), ( char* ) PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_JPEGDECODE:
        return JPEGDecode( ( JPEG* ) PARAM( 0 ), ( COLOR* ) PARAM( 1 ) );

        //----------------------------------------------------------------------
        // RTC 관련
        //----------------------------------------------------------------------
    case SYSCALL_READRTCTIME:
        ReadRTCTime( ( BYTE* ) PARAM( 0 ), ( BYTE* ) PARAM( 1 ), 
                             ( BYTE* ) PARAM( 2 ) );
        return TRUE;
    case SYSCALL_READRTCDATE:
        ReadRTCDate( ( WORD* ) PARAM( 0 ), ( BYTE* ) PARAM( 1 ), 
                             ( BYTE* ) PARAM( 2 ), ( BYTE* ) PARAM( 3 ) );
        return TRUE;

        //----------------------------------------------------------------------
        // 시리얼 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_SENDSERIALDATA:
        return SendSerialData( ( BYTE* ) PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_RECEIVESERIALDATA:
        return ReceiveSerialData( ( BYTE* ) PARAM( 0 ), PARAM( 1 ) );  
    case SYSCALL_CLEARSERIALFIFO:
        ClearSerialFIFO();
        return TRUE;

        //----------------------------------------------------------------------
        // 기타
        //----------------------------------------------------------------------
    case SYSCALL_GETTOTALRAMSIZE:
        return GetTotalRAMSize();
    case SYSCALL_GETTICKCOUNT:
        return GetTickCount();
    case SYSCALL_SLEEP:
        Sleep( PARAM( 0 ) );
        return TRUE;
    case SYSCALL_ISGRAPHICMODE:
        return IsGraphicMode();
    case SYSCALL_TEST:
        Printf( "Test System Call... System Call Test Success~!!\n" );
        return TRUE;
        
        //----------------------------------------------------------------------
        // 정의되지 않은 시스템 콜 처리
        //----------------------------------------------------------------------
    default:
        Printf( "Undefined System Call~!!!, Service Number: %d\n", 
                 qwServiceNumber );
        return FALSE;
    }
}
