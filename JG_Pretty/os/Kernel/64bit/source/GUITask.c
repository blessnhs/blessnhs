/**
 *  file    GUITask.c
 *  date    2009/10/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI 태스크에 관련된 함수를 정의한 소스 파일
 */
#include "GUITask.h"
#include "Window.h"
#include "MultiProcessor.h"
#include "Font.h"
#include "Console.h"
#include "Task.h"
#include "ConsoleShell.h"
#include "JPEG.h"
#include "DynamicMemory.h"
#include "fat32/fat_defs.h"
#include "fat32/fat_filelib.h"
#include "fat32/fat_access.h"


void UpdateWindowManger(QWORD qwWindowID,int count,DIRECTORY *dirlist,RECT *stButtonAreaArray,char *path)
{
    DrawRect( qwWindowID, 0, WINDOW_TITLEBAR_HEIGHT, 800, 600, RGB( 255, 255, 255 ),  TRUE );

    int idx,StartY;
    RECT stButtonArea;

    //Display Current Path
    DrawText( qwWindowID, 20, WINDOW_TITLEBAR_HEIGHT, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),	path, strlen(path ) );

    StartY = WINDOW_TITLEBAR_HEIGHT + 50;

    int fileinfowidth = 160;
    int fileinfoheight = 33;

    //One Line Max Count
    int LineCount = 12;

    for(idx = 0;idx<count;idx++)
    {
    	int yIdx = idx % LineCount;

     	// 이벤트 정보를 표시하는 영역의 테두리와 윈도우 ID를 표시
        DrawRect( qwWindowID,
        		10 + ((idx/LineCount) * fileinfowidth), StartY + (yIdx * fileinfoheight),
        		160 + ((idx/LineCount) * fileinfowidth), StartY + fileinfoheight + (yIdx * fileinfoheight),
				RGB( 0, 0, 0 ),  FALSE );

        SetRectangleData(10 + ((idx/LineCount) * fileinfowidth), StartY + (yIdx * fileinfoheight),
        				160 + ((idx/LineCount) * fileinfowidth), StartY + fileinfoheight + (yIdx * fileinfoheight),
						&stButtonAreaArray[idx] );

    	char vcTempBuffer[1024];

        if(dirlist[idx].is_dir == 1)
        	SPrintf( vcTempBuffer, "[%s]", dirlist[idx].filename );
        else
        	SPrintf( vcTempBuffer, "%s", dirlist[idx].filename );

        DrawText( qwWindowID, 20 + ((idx/LineCount) * fileinfowidth), StartY + 8 + (yIdx * fileinfoheight), RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),vcTempBuffer, strlen( vcTempBuffer ) );
    }
}

void WindowManagerGUITask( void )
{
    if( IsGraphicMode() == FALSE )
    {
        Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }

    QWORD qwWindowID;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;

    char pathBuffer[1024];
    MemSet(pathBuffer,0,1024);

    if( IsGraphicMode() == FALSE )
    {
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
     	Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }

    //current path
    MemCpy(pathBuffer,"/",strlen("/"));

    // 윈도우의 크기와 제목 설정
    iWindowWidth = 800;
    iWindowHeight = 600;

    qwWindowID = CreateWindow( 100, 100,
    iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT, pathBuffer );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
      	Printf( "CreateWindow Failed\n" );
        return ;
    }

    Printf( "Start\n" );

    int count = fl_directoryfilecount(pathBuffer);
    DIRECTORY *dirlist = (DIRECTORY* )fl_listdirectory(pathBuffer);;
    RECT *stButtonAreaArray = NEW(sizeof(RECT) * count);

    UpdateWindowManger(qwWindowID,count,dirlist,stButtonAreaArray,pathBuffer);

    ShowWindow( qwWindowID, TRUE );

    while( 1 )
    {
        if( ReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
            Sleep( 0 );
            continue;
        }
        
        switch( stReceivedEvent.qwType )
        {
        case EVENT_MOUSE_LBUTTONUP:            

        	pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            int idx;
            for(idx = 0;idx<count;idx++)
            {
				if( IsInRectangle( &stButtonAreaArray[idx], pstMouseEvent->stPoint.iX,
													pstMouseEvent->stPoint.iY ) == TRUE )
				{
					DrawText( qwWindowID, 20, WINDOW_TITLEBAR_HEIGHT, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),
							"                                                         ",
							strlen("                                                         ") );

					DrawText( qwWindowID, 20, WINDOW_TITLEBAR_HEIGHT, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),
							dirlist[idx].filename, strlen(dirlist[idx].filename ) );

					if(pathBuffer,dirlist[idx].is_dir == 0)
					{
						char exebuf[1024];
						MemSet(exebuf,0,1024);

						if(strcmp(pathBuffer,"/") == 0)
							SPrintf(exebuf,"exec %s%s",pathBuffer,dirlist[idx].filename);
						else
							SPrintf(exebuf,"exec %s/%s",pathBuffer,dirlist[idx].filename);

						ExecuteCommand(exebuf);
					}
					else
					{
						if(strcmp(dirlist[idx].filename,"..") == 0)
						{
							int count = strlen(pathBuffer);

							int i;
							for(i = count - 1;i>=0 ;i--)
							{
								if(pathBuffer[i] == '/')
								{
									if(i > 0)
										pathBuffer[i] = 0;

									break;
								}
								else
									pathBuffer[i] = 0;
							}
						}
						else
						{
							if(strcmp(pathBuffer,"/") == 0)
								SPrintf(pathBuffer,"%s%s",pathBuffer,dirlist[idx].filename);
							else
							{
								SPrintf(pathBuffer,"%s/%s",pathBuffer,dirlist[idx].filename);
							}
						}

						Printf("pathBuffer %s\n",pathBuffer);

						count = fl_directoryfilecount(pathBuffer);

						if(dirlist != 0)
							FreeMemory(dirlist);

						if(stButtonAreaArray != 0)
							FreeMemory(stButtonAreaArray);

						dirlist = (DIRECTORY* )fl_listdirectory(pathBuffer);;
						stButtonAreaArray = NEW(sizeof(RECT) * count);

						UpdateWindowManger(qwWindowID,count,dirlist,stButtonAreaArray,pathBuffer);
					}

					UpdateScreenByID( qwWindowID );

				}
			}
            break;

            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // 여기에 키보드 이벤트 처리 코드 넣기
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_SELECT:
        case EVENT_WINDOW_DESELECT:
        case EVENT_WINDOW_MOVE:
        case EVENT_WINDOW_RESIZE:
        case EVENT_WINDOW_CLOSE:
            // 여기에 윈도우 이벤트 처리 코드 넣기
            pstWindowEvent = &( stReceivedEvent.stWindowEvent );

            //------------------------------------------------------------------
            // 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
            //------------------------------------------------------------------
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                DeleteWindow( qwWindowID );
                return ;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }
}

//------------------------------------------------------------------------------
//  Hello World GUI 태스크
//------------------------------------------------------------------------------
/**
 *  Hello World GUI 태스크
 */
void HelloWorldGUITask( void )
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;
    int iY;
    char vcTempBuffer[ 50 ];
    static int s_iWindowCount = 0;
    // 이벤트 타입 문자열
    char* vpcEventString[] = { 
            "Unknown",
            "MOUSE_MOVE       ",
            "MOUSE_LBUTTONDOWN",
            "MOUSE_LBUTTONUP  ",
            "MOUSE_RBUTTONDOWN",
            "MOUSE_RBUTTONUP  ",
            "MOUSE_MBUTTONDOWN",
            "MOUSE_MBUTTONUP  ",
            "WINDOW_SELECT    ",
            "WINDOW_DESELECT  ",
            "WINDOW_MOVE      ",
            "WINDOW_RESIZE    ",
            "WINDOW_CLOSE     ",            
            "KEY_DOWN         ",
            "KEY_UP           " };
    RECT stButtonArea;
    QWORD qwFindWindowID;
    EVENT stSendEvent;
    int i;

    //--------------------------------------------------------------------------
    // 그래픽 모드 판단
    //--------------------------------------------------------------------------
    // MINT64 OS가 그래픽 모드로 시작했는지 확인
    if( IsGraphicMode() == FALSE )
    {        
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 마우스의 현재 위치를 반환
    GetCursorPosition( &iMouseX, &iMouseY );

    // 윈도우의 크기와 제목 설정
    iWindowWidth = 500;
    iWindowHeight = 200;
    
    // 윈도우 생성 함수 호출, 마우스가 있던 위치를 기준으로 생성하고 번호를 추가하여
    // 윈도우마다 개별적인 이름을 할당
    SPrintf( vcTempBuffer, "Hello World Window %d", s_iWindowCount++ );
    qwWindowID = CreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT, vcTempBuffer );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우 매니저가 윈도우로 전송하는 이벤트를 표시하는 영역을 그림
    //--------------------------------------------------------------------------
    // 이벤트 정보를 출력할 위치 저장
    iY = WINDOW_TITLEBAR_HEIGHT + 10;
    
    // 이벤트 정보를 표시하는 영역의 테두리와 윈도우 ID를 표시
    DrawRect( qwWindowID, 10, iY + 8, iWindowWidth - 10, iY + 70, RGB( 0, 0, 0 ),
            FALSE );
    SPrintf( vcTempBuffer, "GUI Event Information[Window ID: 0x%Q]", qwWindowID );
    DrawText( qwWindowID, 20, iY, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ), 
               vcTempBuffer, kStrLen( vcTempBuffer ) );    
    
    //--------------------------------------------------------------------------
    // 화면 아래에 이벤트 전송 버튼을 그림
    //--------------------------------------------------------------------------
    // 버튼 영역을 설정
    SetRectangleData( 10, iY + 80, iWindowWidth - 10, iWindowHeight - 10, 
            &stButtonArea );
    // 배경은 윈도우의 배경색으로 설정하고 문자는 검은색으로 설정하여 버튼을 그림
    DrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, 
            "User Message Send Button(Up)", RGB( 0, 0, 0 ) );
    // 윈도우를 화면에 표시
    ShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( ReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
            Sleep( 0 );
            continue;
        }
        
        // 윈도우 이벤트 정보가 표시될 영역을 배경색으로 칠하여 이전에 표시한 데이터를
        // 모두 지움
        DrawRect( qwWindowID, 11, iY + 20, iWindowWidth - 11, iY + 69, 
                   WINDOW_COLOR_BACKGROUND, TRUE );        
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 이벤트 처리
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_LBUTTONDOWN:
        case EVENT_MOUSE_LBUTTONUP:            
        case EVENT_MOUSE_RBUTTONDOWN:
        case EVENT_MOUSE_RBUTTONUP:
        case EVENT_MOUSE_MBUTTONDOWN:
        case EVENT_MOUSE_MBUTTONUP:
            // 여기에 마우스 이벤트 처리 코드 넣기
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // 마우스 이벤트의 타입을 출력
            SPrintf( vcTempBuffer, "Mouse Event: %s", 
                      vpcEventString[ stReceivedEvent.qwType ] );
            DrawText( qwWindowID, 20, iY + 20, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            
            // 마우스 데이터를 출력
            SPrintf( vcTempBuffer, "Data: X = %d, Y = %d, Button = %X", 
                     pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY,
                     pstMouseEvent->bButtonStatus );
            DrawText( qwWindowID, 20, iY + 40, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );

            //------------------------------------------------------------------
            // 마우스 눌림 또는 떨어짐 이벤트이면 버튼의 색깔을 다시 그림
            //------------------------------------------------------------------
            // 마우스 왼쪽 버튼이 눌렸을 때 버튼 처리
            if( stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONDOWN )
            {
                // 버튼 영역에 마우스 왼쪽 버튼이 눌렸는지를 판단
                if( IsInRectangle( &stButtonArea, pstMouseEvent->stPoint.iX, 
                                    pstMouseEvent->stPoint.iY ) == TRUE )
                {
                    // 버튼의 배경을 밝은 녹색으로 변경하여 눌렸음을 표시
                    DrawButton( qwWindowID, &stButtonArea, 
                                 RGB( 79, 204, 11 ), "User Message Send Button(Down)",
                                 RGB( 255, 255, 255 ) );
                    UpdateScreenByID( qwWindowID );
                    
                    //----------------------------------------------------------
                    // 다른 윈도우로 유저 이벤트를 전송
                    //----------------------------------------------------------
                    // 생성된 모든 윈도우를 찾아서 이벤트를 전송
                    stSendEvent.qwType = EVENT_USER_TESTMESSAGE;
                    stSendEvent.vqwData[ 0 ] = qwWindowID;
                    stSendEvent.vqwData[ 1 ] = 0x1234;
                    stSendEvent.vqwData[ 2 ] = 0x5678;
                    
                    // 생성된 윈도우의 수 만큼 루프를 수행하면서 이벤트를 전송
                    for( i = 0 ; i < s_iWindowCount ; i++ )
                    {
                        // 윈도우 제목으로 윈도우를 검색
                        SPrintf( vcTempBuffer, "Hello World Window %d", i );
                        qwFindWindowID = FindWindowByTitle( vcTempBuffer );
                        // 윈도우가 존재하며 윈도우 자신이 아닌 경우는 이벤트를 전송
                        if( ( qwFindWindowID != WINDOW_INVALIDID ) &&
                            ( qwFindWindowID != qwWindowID ) )
                        {
                            // 윈도우로 이벤트 전송
                            SendEventToWindow( qwFindWindowID, &stSendEvent );
                        }
                    }
                }
            }
            // 마우스 왼쪽 버튼이 떨어졌을 때 버튼 처리
            else if( stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONUP )
            {
                // 버튼의 배경을 흰색으로 변경하여 눌리지 않았음을 표시
                DrawButton( qwWindowID, &stButtonArea, 
                    WINDOW_COLOR_BACKGROUND, "User Message Send Button(Up)", 
                    RGB( 0, 0, 0 ) );
            }            
            break;

            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // 여기에 키보드 이벤트 처리 코드 넣기
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );

            // 키 이벤트의 타입을 출력
            SPrintf( vcTempBuffer, "Key Event: %s", 
                      vpcEventString[ stReceivedEvent.qwType ] );
            DrawText( qwWindowID, 20, iY + 20, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            
            // 키 데이터를 출력
            SPrintf( vcTempBuffer, "Data: Key = %c, Flag = %X", 
                    pstKeyEvent->bASCIICode, pstKeyEvent->bFlags );
            DrawText( qwWindowID, 20, iY + 40, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_SELECT:
        case EVENT_WINDOW_DESELECT:
        case EVENT_WINDOW_MOVE:
        case EVENT_WINDOW_RESIZE:
        case EVENT_WINDOW_CLOSE:
            // 여기에 윈도우 이벤트 처리 코드 넣기
            pstWindowEvent = &( stReceivedEvent.stWindowEvent );

            // 윈도우 이벤트의 타입을 출력
            SPrintf( vcTempBuffer, "Window Event: %s", 
                      vpcEventString[ stReceivedEvent.qwType ] );
            DrawText( qwWindowID, 20, iY + 20, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            
            // 윈도우 데이터를 출력
            SPrintf( vcTempBuffer, "Data: X1 = %d, Y1 = %d, X2 = %d, Y2 = %d", 
                    pstWindowEvent->stArea.iX1, pstWindowEvent->stArea.iY1, 
                    pstWindowEvent->stArea.iX2, pstWindowEvent->stArea.iY2 );
            DrawText( qwWindowID, 20, iY + 40, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            
            //------------------------------------------------------------------
            // 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
            //------------------------------------------------------------------
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                DeleteWindow( qwWindowID );
                return ;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            // 알 수 없는 이벤트를 출력
            SPrintf( vcTempBuffer, "Unknown Event: 0x%X", stReceivedEvent.qwType );
            DrawText( qwWindowID, 20, iY + 20, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
                       vcTempBuffer, kStrLen( vcTempBuffer ) );
            
            // 데이터를 출력
            SPrintf( vcTempBuffer, "Data0 = 0x%Q, Data1 = 0x%Q, Data2 = 0x%Q",
                      stReceivedEvent.vqwData[ 0 ], stReceivedEvent.vqwData[ 1 ], 
                      stReceivedEvent.vqwData[ 2 ] );
            DrawText( qwWindowID, 20, iY + 40, RGB( 0, 0, 0 ), 
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, kStrLen( vcTempBuffer ) );
            break;
        }

        // 윈도우를 화면에 업데이트
        ShowWindow( qwWindowID, TRUE );
    }
}

//------------------------------------------------------------------------------
//  시스템 모니터 태스크
//------------------------------------------------------------------------------
/**
 *  시스템의 상태를 감시하여 화면에 표시하는 태스크
 */
void SystemMonitorTask( void )
{
    QWORD qwWindowID;
    int i;
    int iWindowWidth;
    int iProcessorCount;
    DWORD vdwLastCPULoad[ MAXPROCESSORCOUNT ];
    int viLastTaskCount[ MAXPROCESSORCOUNT ];
    QWORD qwLastTickCount;
    EVENT stReceivedEvent;
    WINDOWEVENT* pstWindowEvent;
    BOOL bChanged;
    RECT stScreenArea;
    QWORD qwLastDynamicMemoryUsedSize;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwTemp;
    
    //--------------------------------------------------------------------------
    // 그래픽 모드 판단
    //--------------------------------------------------------------------------
    // MINT64 OS가 그래픽 모드로 시작했는지 확인
    if( IsGraphicMode() == FALSE )
    {        
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 화면 영역의 크기를 반환
    GetScreenArea( &stScreenArea );
    
    // 현재 프로세서의 개수로 윈도우의 너비를 계산
    iProcessorCount = GetProcessorCount();
    iWindowWidth = iProcessorCount * ( SYSTEMMONITOR_PROCESSOR_WIDTH + 
            SYSTEMMONITOR_PROCESSOR_MARGIN ) + SYSTEMMONITOR_PROCESSOR_MARGIN;
    
    // 윈도우를 화면 가운데에 생성한 뒤 화면에 표시하지 않음. 프로세서 정보와 메모리 정보를 
    // 표시하는 영역을 그린 뒤 화면에 표시
    qwWindowID = CreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - SYSTEMMONITOR_WINDOW_HEIGHT ) / 2, 
        iWindowWidth, SYSTEMMONITOR_WINDOW_HEIGHT, WINDOW_FLAGS_DEFAULT & 
        ~WINDOW_FLAGS_SHOW, "System Monitor" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }

    // 프로세서 정보를 표시하는 영역을 3픽셀 두께로 표시하고 문자열을 출력
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 15, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 15, RGB( 0, 0, 0 ) );
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 16, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 16, RGB( 0, 0, 0 ) );
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 17, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 17, RGB( 0, 0, 0 ) );
    DrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + 8, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "Processor Information", 21 );


    // 메모리 정보를 표시하는 영역을 3픽셀 두께로 표시하고 문자열을 출력
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            RGB( 0, 0, 0 ) );
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                RGB( 0, 0, 0 ) );
    DrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                RGB( 0, 0, 0 ) );
    DrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 43, 
            RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "Memory Information", 18 );
    // 윈도우를 화면에 표시
    ShowWindow( qwWindowID, TRUE );
    
    // 루프를 돌면서 시스템 정보를 감시하여 화면에 표시
    qwLastTickCount = 0;
    
    // 마지막으로 측정한 프로세서의 부하와 태스크 수, 그리고 메모리 사용량은 모두 0으로 설정
    MemSet( vdwLastCPULoad, 0, sizeof( vdwLastCPULoad ) );
    MemSet( viLastTaskCount, 0, sizeof( viLastTaskCount ) );
    qwLastDynamicMemoryUsedSize = 0;
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        //----------------------------------------------------------------------
        // 이벤트 큐의 이벤트 처리
        //----------------------------------------------------------------------
        // 이벤트 큐에서 이벤트를 수신
        if( ReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == TRUE )
        {
            // 수신된 이벤트를 타입에 따라 나누어 처리
            switch( stReceivedEvent.qwType )
            {
                // 윈도우 이벤트 처리
            case EVENT_WINDOW_CLOSE:
                //--------------------------------------------------------------
                // 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
                //--------------------------------------------------------------
                // 윈도우 삭제
                DeleteWindow( qwWindowID );
                return ;
                break;
                
                // 그 외 정보
            default:
                break;
            }
        }
        
        // 0.5초마다 한번씩 시스템 상태를 확인
        if( ( GetTickCount() - qwLastTickCount ) < 500 )
        {
            Sleep( 1 );
            continue;
        }

        // 마지막으로 측정한 시간을 최신으로 업데이트
        qwLastTickCount = GetTickCount();

        //----------------------------------------------------------------------
        // 프로세서 정보 출력
        //----------------------------------------------------------------------
        // 프로세서 수만큼 부하와 태스크 수를 확인하여 달라진 점이 있으면 화면에 업데이트
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            bChanged = FALSE;
            
            // 프로세서 부하 검사
            if( vdwLastCPULoad[ i ] != GetProcessorLoad( i ) )
            {
                vdwLastCPULoad [ i ] = GetProcessorLoad( i );
                bChanged = TRUE;
            }
            // 태스크 수 검사
            else if( viLastTaskCount[ i ] != GetTaskCount( i ) )
            {
                viLastTaskCount[ i ] = GetTaskCount( i );
                bChanged = TRUE;
            }
            
            // 이전과 비교해서 변경 사항이 있으면 화면에 업데이트
            if( bChanged == TRUE )
            {
                // 화면에 현재 프로세서의 부하를 표시 
                DrawProcessorInformation( qwWindowID, i * SYSTEMMONITOR_PROCESSOR_WIDTH + 
                    ( i + 1 ) * SYSTEMMONITOR_PROCESSOR_MARGIN, WINDOW_TITLEBAR_HEIGHT + 28,
                    i );
            }
        }
        
        //----------------------------------------------------------------------
        // 동적 메모리 정보 출력
        //----------------------------------------------------------------------
        // 동적 메모리의 정보를 반환
        GetDynamicMemoryInformation( &qwTemp, &qwTemp, &qwTemp, 
                &qwDynamicMemoryUsedSize );
        
        // 현재 동적 할당 메모리 사용량이 이전과 다르다면 메모리 정보를 출력
        if( qwDynamicMemoryUsedSize != qwLastDynamicMemoryUsedSize )
        {
            qwLastDynamicMemoryUsedSize = qwDynamicMemoryUsedSize;
            
            // 메모리 정보를 출력
            DrawMemoryInformation( qwWindowID, WINDOW_TITLEBAR_HEIGHT + 
                    SYSTEMMONITOR_PROCESSOR_HEIGHT + 60, iWindowWidth );
        }
    }
}

/**
 *  개별 프로세서의 정보를 화면에 표시
 */
static void DrawProcessorInformation( QWORD qwWindowID, int iX, int iY, 
        BYTE bAPICID )
{
    char vcBuffer[ 100 ];
    RECT stArea;
    QWORD qwProcessorLoad;
    QWORD iUsageBarHeight;
    int iMiddleX;
    
    // 프로세서 ID를 표시
    SPrintf( vcBuffer, "Processor ID: %d", bAPICID );
    DrawText( qwWindowID, iX + 10, iY, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, kStrLen( vcBuffer ) );
    
    // 프로세서의 태스크 개수를 표시
    SPrintf( vcBuffer, "Task Count: %d   ", GetTaskCount( bAPICID ) );
    DrawText( qwWindowID, iX + 10, iY + 18, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
            vcBuffer, kStrLen( vcBuffer ) );

    //--------------------------------------------------------------------------
    // 프로세서 부하를 나타내는 막대를 표시
    //--------------------------------------------------------------------------
    // 프로세서 부하를 표시
    qwProcessorLoad = GetProcessorLoad( bAPICID );
    if( qwProcessorLoad > 100 )
    {
        qwProcessorLoad = 100;
    }
    
    // 부하를 표시하는 막대의 전체에 테두리를 표시
    DrawRect( qwWindowID, iX, iY + 36, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, RGB( 0, 0, 0 ), FALSE );

    // 프로세서 사용량을 나타내는 막대의 길이, ( 막대 전체의 길이 * 프로세서 부하 / 100 ) 
    iUsageBarHeight = ( SYSTEMMONITOR_PROCESSOR_HEIGHT - 40 ) * qwProcessorLoad / 100;

    // 부하를 표시하는 영역의 막대 내부를 표시
    // 채워진 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠 
    DrawRect( qwWindowID, iX + 2,
        iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 2, 
        iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2, 
        iY + SYSTEMMONITOR_PROCESSOR_HEIGHT - 2, SYSTEMMONITOR_BAR_COLOR, TRUE );
    // 빈 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠
    DrawRect( qwWindowID, iX + 2, iY + 38, iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2,
            iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 1, 
            WINDOW_COLOR_BACKGROUND, TRUE );
    
    // 프로세서의 부하를 표시, 막대의 가운데에 부하가 표시되도록 함
    SPrintf( vcBuffer, "Usage: %d%%", qwProcessorLoad );
    iMiddleX = ( SYSTEMMONITOR_PROCESSOR_WIDTH - 
            ( kStrLen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    DrawText( qwWindowID, iX + iMiddleX, iY + 80, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen( vcBuffer ) );
    
    // 프로세서 정보가 표시된 영역만 다시 화면에 업데이트
    SetRectangleData( iX, iY, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, &stArea );    
    UpdateScreenByWindowArea( qwWindowID, &stArea );
}

/**
 *  메모리 정보를 출력
 */
static void DrawMemoryInformation( QWORD qwWindowID, int iY, int iWindowWidth )
{
    char vcBuffer[ 100 ];
    QWORD qwTotalRAMKbyteSize;
    QWORD qwDynamicMemoryStartAddress;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwUsedPercent;
    QWORD qwTemp;
    int iUsageBarWidth;
    RECT stArea;
    int iMiddleX;
    
    // Mbyte 단위의 메모리를 Kbyte 단위로 변환
    qwTotalRAMKbyteSize = GetTotalRAMSize() * 1024;
    
    // 메모리 정보를 표시
    SPrintf( vcBuffer, "Total Size: %d KB        ", qwTotalRAMKbyteSize );
    DrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 3, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen( vcBuffer ) );
    
    // 동적 메모리의 정보를 반환
    GetDynamicMemoryInformation( &qwDynamicMemoryStartAddress, &qwTemp, 
            &qwTemp, &qwDynamicMemoryUsedSize );
    
    SPrintf( vcBuffer, "Used Size: %d KB        ", ( qwDynamicMemoryUsedSize + 
            qwDynamicMemoryStartAddress ) / 1024 );
    DrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 21, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, kStrLen( vcBuffer ) );
    
    //--------------------------------------------------------------------------
    // 메모리 사용량을 나타내는 막대를 표시
    //--------------------------------------------------------------------------
    // 메모리 사용량을 표시하는 막대의 전체에 테두리를 표시
    DrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN, iY + 40,
            iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 32, RGB( 0, 0, 0 ), FALSE );
    // 메모리 사용량(%)을 계산
    qwUsedPercent = ( qwDynamicMemoryStartAddress + qwDynamicMemoryUsedSize ) * 
        100 / 1024 / qwTotalRAMKbyteSize;
    if( qwUsedPercent > 100 )
    {
        qwUsedPercent = 100;
    }
    
    // 메모리 사용량을 나타내는 막대의 길이, ( 막대 전체의 길이 * 메모리 사용량 / 100 )     
    iUsageBarWidth = ( iWindowWidth - 2 * SYSTEMMONITOR_PROCESSOR_MARGIN ) * 
        qwUsedPercent / 100;
    
    // 메모리 사용량을 표시하는 영역의 막대 내부를 표시
    // 색칠된 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠 
    DrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2, iY + 42, 
            SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, SYSTEMMONITOR_BAR_COLOR, TRUE );  
    // 빈 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠
    DrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
        iY + 42, iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN - 2,
        iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, WINDOW_COLOR_BACKGROUND, TRUE );    
    
    // 사용량을 나타내는 텍스트 표시, 막대의 가운데에 사용량이 표시되도록 함
    SPrintf( vcBuffer, "Usage: %d%%", qwUsedPercent );
    iMiddleX = ( iWindowWidth - ( kStrLen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    DrawText( qwWindowID, iMiddleX, iY + 45, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, kStrLen( vcBuffer ) );
    
    // 메모리 정보가 표시된 영역만 화면에 다시 업데이트
    SetRectangleData(0, iY, iWindowWidth, iY + SYSTEMMONITOR_MEMORY_HEIGHT, &stArea );
    UpdateScreenByWindowArea( qwWindowID, &stArea );
}

//------------------------------------------------------------------------------
//  GUI 버전의 콘솔 셸 태스크
//------------------------------------------------------------------------------
// 이전 화면 버퍼의 값을 보관하는 영역
static CHARACTER gs_vstPreviousScreenBuffer[ CONSOLE_WIDTH * CONSOLE_HEIGHT ];

/**
 *  GUI 버전의 콘솔 셸 태스크
 */
void GUIConsoleShellTask( void )
{
    static QWORD s_qwWindowID = WINDOW_INVALIDID;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    RECT stScreenArea;
    KEY_DATA stKEY_DATA;
    TCB* pstConsoleShellTask;
    QWORD qwConsoleShellTaskID;

    //--------------------------------------------------------------------------
    // 그래픽 모드 판단
    //--------------------------------------------------------------------------
    // MINT64 OS가 그래픽 모드로 시작했는지 확인
    if( IsGraphicMode() == FALSE )
    {        
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }
    
    // GUI 콘솔 셸 윈도우가 존재하면 생성된 윈도우를 최상위로 만들고 태스크 종료
    if( s_qwWindowID != WINDOW_INVALIDID )
    {
        MoveWindowToTop( s_qwWindowID );
        return ;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 전체 화면 영역의 크기를 반환
    GetScreenArea( &stScreenArea );
    
    // 윈도우의 크기 설정, 화면 버퍼에 들어가는 문자의 최대 너비와 높이를 고려해서 계산
    iWindowWidth = CONSOLE_WIDTH * FONT_ENGLISHWIDTH + 4;
    iWindowHeight = CONSOLE_HEIGHT * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT + 2;
    // 윈도우 생성 함수 호출, 화면 가운데에 생성
    s_qwWindowID = CreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT, "Console Shell for GUI" );
    // 윈도우를 생성하지 못했으면 실패
    if( s_qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }

    // 셸 커맨드를 처리하는 콘솔 셸 태스크를 생성
    SetConsoleShellExitFlag( FALSE );
    pstConsoleShellTask = CreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, 
        ( QWORD ) StartConsoleShell, TASK_LOADBALANCINGID );
    if( pstConsoleShellTask == NULL )
    {
        // 콘솔 셸 태스크 생성에 실패하면 윈도우를 삭제하고 종료
        DeleteWindow( s_qwWindowID );
        return ;
    }
    // 콘솔 셸 태스크의 ID를 저장
    qwConsoleShellTaskID = pstConsoleShellTask->stLink.qwID;

    // 이전 화면 버퍼를 초기화
    MemSet( gs_vstPreviousScreenBuffer, 0xFF, sizeof( gs_vstPreviousScreenBuffer ) );
    
    ShowWindow( qwConsoleShellTaskID, TRUE );

    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 화면 버퍼의 변경된 내용을 윈도우에 업데이트
        ProcessConsoleBuffer( s_qwWindowID );
        
        // 이벤트 큐에서 이벤트를 수신
        if( ReceiveEventFromWindowQueue( s_qwWindowID, &stReceivedEvent ) == FALSE )
        {
            Sleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // 여기에 키보드 이벤트 처리 코드 넣기
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            stKEY_DATA.bASCIICode = pstKeyEvent->bASCIICode;
            stKEY_DATA.bFlags = pstKeyEvent->bFlags;
            stKEY_DATA.bScanCode = pstKeyEvent->bScanCode;

            // 키를 그래픽 모드용 키 큐로 삽입하여 셸 태스크의 입력으로 전달
            PutKeyToGUIKeyQueue( &stKEY_DATA );
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_CLOSE:
            // 생성한 셸 태스크가 종료되도록 종료 플래그를 설정하고 종료될 때까지 대기
            SetConsoleShellExitFlag( TRUE );
            while( IsTaskExist( qwConsoleShellTaskID ) == TRUE )
            {
                Sleep( 1 );
            }
            
            // 윈도우를 삭제하고 윈도우 ID를 초기화
            DeleteWindow( s_qwWindowID );
            s_qwWindowID = WINDOW_INVALIDID;            
            return ;
            
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }
}

/**
 *  화면 버퍼의 변경된 내용을 GUI 콘솔 셸 윈도우 화면에 업데이트
 */
static void ProcessConsoleBuffer( QWORD qwWindowID )
{
    int i;
    int j;
    CONSOLEMANAGER* pstManager;
    CHARACTER* pstScreenBuffer;
    CHARACTER* pstPreviousScreenBuffer;
    RECT stLineArea;
    BOOL bChanged;
    static QWORD s_qwLastTickCount = 0;
    BOOL bFullRedraw;
    
    // 콘솔을 관리하는 자료구조를 반환 받아 화면 버퍼의 어드레스를 저장하고 
    // 이전 화면 버퍼의 어드레스도 저장
    pstManager = GetConsoleManager();
    pstScreenBuffer = pstManager->pstScreenBuffer;
    pstPreviousScreenBuffer = gs_vstPreviousScreenBuffer;
    
    // 화면을 전체를 업데이트 한 지 5초가 지났으면 무조건 화면 전체를 다시 그림
    if( GetTickCount() - s_qwLastTickCount > 5000 )
    {
        s_qwLastTickCount = GetTickCount();
        bFullRedraw = TRUE;
    }
    else
    {
        bFullRedraw = FALSE;
    }

    // 화면 버퍼의 높이만큼 반복
    for( j = 0 ; j < CONSOLE_HEIGHT ; j++ )
    {
        // 처음에는 변경되지 않은 것으로 플래그 설정
        bChanged = FALSE;
        
        // 현재 라인에 변화가 있는지 비교하여 변경 여부 플래그를 처리
        for( i = 0 ; i < CONSOLE_WIDTH ; i++ )
        {
            // 문자를 비교하여 다르거나 전체를 새로 그려야 하면 이전 화면 버퍼에
            // 업데이트하고 변경 여부 플래그를 설정
            if( ( pstScreenBuffer->bCharactor != pstPreviousScreenBuffer->bCharactor ) ||
                ( bFullRedraw == TRUE ) )
            {
                // 문자를 화면에 출력
                DrawText( qwWindowID, i * FONT_ENGLISHWIDTH + 2, 
                           j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT, 
                           RGB( 255, 255, 255 ), RGB( 0, 0, 0 ),
                           &( pstScreenBuffer->bCharactor ), 1);
                
                // 이전 화면 버퍼로 값을 옮겨 놓고 현재 라인에 이전과
                // 다른 데이터가 있음을 표시
                MemCpy( pstPreviousScreenBuffer, pstScreenBuffer, sizeof( CHARACTER ) );
                bChanged = TRUE;
            }
            
            pstScreenBuffer++;
            pstPreviousScreenBuffer++;
        }
        
        // 현재 라인에서 변경된 데이터가 있다면 현재 라인만 화면에 업데이트
        if( bChanged == TRUE )
        {
            // 현재 라인의 영역을 계산
            SetRectangleData( 2, j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT,
                5 + FONT_ENGLISHWIDTH * CONSOLE_WIDTH, ( j + 1 ) * FONT_ENGLISHHEIGHT + 
                WINDOW_TITLEBAR_HEIGHT - 1, &stLineArea );
            // 윈도우의 일부만 화면 업데이트
            UpdateScreenByWindowArea( qwWindowID, &stLineArea );
        }
    }
}

//------------------------------------------------------------------------------
//  이미지 뷰어(Image Viewer)
//------------------------------------------------------------------------------
/**
 *  이미지 뷰어 태스크
 */
void ImageViewerTask( void )
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    int iEditBoxWidth;
    RECT stEditBoxArea;
    RECT stButtonArea;
    RECT stScreenArea;
    EVENT stReceivedEvent;
    EVENT stSendEvent;
    char vcFileName[ FILESYSTEM_MAXFILENAMELENGTH + 1 ];
    int iFileNameLength;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    POINT stScreenXY;
    POINT stClientXY;

    //--------------------------------------------------------------------------
    // 그래픽 모드 판단
    //--------------------------------------------------------------------------
    // MINT64 OS가 그래픽 모드로 시작했는지 확인
    if( IsGraphicMode() == FALSE )
    {        
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        Printf( "This task can run only GUI mode~!!!\n" );
        return ;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 전체 화면 영역의 크기를 반환
    GetScreenArea( &stScreenArea );
    
    // 윈도우의 크기 설정, 화면 버퍼에 들어가는 문자의 최대 너비와 높이를 고려해서 계산
    iWindowWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 165;
    iWindowHeight = 35 + WINDOW_TITLEBAR_HEIGHT + 5;
    
    // 윈도우 생성 함수 호출, 화면 가운데에 생성
    qwWindowID = CreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW, "Image Viewer" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }
    
    // 파일 이름을 입력하는 에디트 박스 영역을 표시
    DrawText( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 6, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "FILE NAME", 9 );
    iEditBoxWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 4;
    SetRectangleData( 85, WINDOW_TITLEBAR_HEIGHT + 5, 85 + iEditBoxWidth, 
                       WINDOW_TITLEBAR_HEIGHT + 25, &stEditBoxArea );
    DrawRect( qwWindowID, stEditBoxArea.iX1, stEditBoxArea.iY1, 
            stEditBoxArea.iX2, stEditBoxArea.iY2, RGB( 0, 0, 0 ), FALSE );

    // 파일 이름 버퍼를 비우고 에디트 박스에 빈 파일 이름을 표시
    iFileNameLength = 0;
    MemSet( vcFileName, 0, sizeof( vcFileName ) );
    DrawFileName( qwWindowID, &stEditBoxArea, vcFileName, iFileNameLength );
    
    // 이미지 출력 버튼 영역을 지정
    SetRectangleData( stEditBoxArea.iX2 + 10, stEditBoxArea.iY1, 
                       stEditBoxArea.iX2 + 70, stEditBoxArea.iY2, &stButtonArea );
    DrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, "Show", 
            RGB( 0, 0, 0 ) );
    
    // 윈도우를 표시
    ShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( ReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
            Sleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 이벤트 처리
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // 마우스 왼쪽 버튼이 이미지 출력 버튼 위에서 눌러졌으면 저장된 파일 이름을 
            // 이용하여 이미지를 화면에 표시
            if( IsInRectangle( &stButtonArea, pstMouseEvent->stPoint.iX, 
                                pstMouseEvent->stPoint.iY ) == TRUE )
            {
                // 버튼을 눌린 것으로 표시
                DrawButton( qwWindowID, &stButtonArea, RGB( 79, 204, 11 ), "Show", 
                            RGB( 255, 255, 255 ) );
                // 버튼이 있는 영역만 화면 업데이트
                UpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );

                // 이미지 출력 윈도우를 생성하고 이벤트를 처리
                if( CreateImageViewerWindowAndExecute( qwWindowID, vcFileName ) 
                        == FALSE )
                {
                    // 윈도우 생성에 실패하면 버튼이 눌려졌다가 떨어지는 효과를 주려고
                    // 200ms 대기
                    Sleep( 200 );
                }
                
                // 버튼을 떨어진 것으로 표시
                DrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
                        "Show", RGB( 0, 0, 0 ) );
                // 버튼이 있는 영역만 화면 업데이트
                UpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );
            }
            break;

            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            
            // 백스페이스(Backspace) 키는 삽입된 문자를 삭제
            if( ( pstKeyEvent->bASCIICode == KEY_BACKSPACE ) &&
                ( iFileNameLength > 0 ) )
            {
                // 버퍼에 삽입된 마지막 문자를 삭제
                vcFileName[ iFileNameLength ] = '\0';
                iFileNameLength--;
                
                // 입력된 내용을 에디트 박스에 표시
                DrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }
            // 엔터(Enter) 키는 이미지 출력 버튼이 눌린 것으로 처리
            else if( ( pstKeyEvent->bASCIICode == KEY_ENTER ) &&
                     ( iFileNameLength > 0 ) )
            {
                // 버튼의 XY 좌표를 화면 좌표로 변환하여 마우스 이벤트의 좌표로 사용
                stClientXY.iX = stButtonArea.iX1 + 1;
                stClientXY.iY = stButtonArea.iY1 + 1;                
                ConvertPointClientToScreen( qwWindowID, &stClientXY, &stScreenXY );
                
                // 이미지 출력 버튼에 마우스 왼쪽 버튼이 눌린 것처럼 마우스 이벤트를 전송
                SetMouseEvent( qwWindowID, EVENT_MOUSE_LBUTTONDOWN, 
                                stScreenXY.iX + 1, stScreenXY.iY + 1, 0, &stSendEvent );
                SendEventToWindow( qwWindowID, &stSendEvent );
            }
            // ESC 키는 윈도우 닫힘 버튼이 눌린 것으로 처리
            else if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                // 윈도우 닫기 이벤트를 윈도우로 전송
                SetWindowEvent( qwWindowID, EVENT_WINDOW_CLOSE, &stSendEvent );
                SendEventToWindow( qwWindowID, &stSendEvent );
            }
            // 그 외 키는 파일 이름 버퍼에 공간이 있는 경우만 버퍼에 삽입
            else if( ( pstKeyEvent->bASCIICode <= 128 ) && 
                     ( pstKeyEvent->bASCIICode != KEY_BACKSPACE ) &&
                     ( iFileNameLength < FILESYSTEM_MAXFILENAMELENGTH ) )
            {
                // 입력된 키를 파일 이름 버퍼의 마지막에 삽입
                vcFileName[ iFileNameLength ] = pstKeyEvent->bASCIICode;
                iFileNameLength++;
                
                // 입력된 내용을 에디트 박스에 표시
                DrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }            
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_CLOSE:
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                DeleteWindow( qwWindowID );
                return ;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }
}

/**
 *  에디트 박스 영역에 문자를 출력
 */
static void DrawFileName( QWORD qwWindowID, RECT* pstArea, char *pcFileName, 
        int iNameLength )
{
    // 에디트 박스의 배경을 모두 흰색으로 채움
    DrawRect( qwWindowID, pstArea->iX1 + 1, pstArea->iY1 + 1, pstArea->iX2 - 1, 
               pstArea-> iY2 - 1, WINDOW_COLOR_BACKGROUND, TRUE );
    
    // 파일 이름을 출력
    DrawText( qwWindowID, pstArea->iX1 + 2, pstArea->iY1 + 2, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, pcFileName, iNameLength );
    
    // 파일 이름의 길이가 파일 시스템이 정의한 최대 길이가 아니면 커서를 출력
    if( iNameLength < FILESYSTEM_MAXFILENAMELENGTH )
    {
        DrawText( qwWindowID, pstArea->iX1 + 2 + FONT_ENGLISHWIDTH * iNameLength,
            pstArea->iY1 + 2, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "_", 1 );
    }
    
    // 에디트 박스 영역만 화면 업데이트
    UpdateScreenByWindowArea( qwWindowID, pstArea );
}

/**
 *  JPEG 파일을 읽어서 새로 생성한 윈도우에 표시하고 이벤트를 처리
 */
static BOOL CreateImageViewerWindowAndExecute( QWORD qwMainWindowID, 
        const char* pcFileName )
{
    struct dirent* pstEntry;
    DWORD dwFileSize;
    RECT stScreenArea;
    QWORD qwWindowID;
    WINDOW* pstWindow;
    BYTE* pbFileBuffer;
    COLOR* pstOutputBuffer;
    int iWindowWidth;
    FL_FILE* fp;
    JPEG* pstJpeg;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    BOOL bExit;
    
    // 초기화
    fp = NULL;
    pbFileBuffer = NULL;
    pstOutputBuffer = NULL;
    qwWindowID = WINDOW_INVALIDID;
    
    dwFileSize  = 0;
    
    //--------------------------------------------------------------------------
    //  파일을 읽은 후 이미지 디코딩
    //--------------------------------------------------------------------------
    // 파일 읽기
    fp = fl_fopen( pcFileName, "rb" );
    if( fp == NULL )
    {
        Printf( "[ImageViewer] %s file open fail\n", pcFileName );
        return FALSE;
    }
    
    dwFileSize = fp->filelength;

    // 메모리를 파일 크기만큼 할당하고 JPEG 자료구조를 할당
    pbFileBuffer = ( BYTE* ) AllocateMemory( dwFileSize );
    pstJpeg = ( JPEG* ) AllocateMemory( sizeof( JPEG ) );
    if( ( pbFileBuffer == NULL ) || ( pstJpeg == NULL ) )
    {
        Printf( "[ImageViewer] Buffer allocation fail\n" );
        FreeMemory( pbFileBuffer );
        FreeMemory( pstJpeg );
        fl_fclose( fp );
        return FALSE;
    }
    
    // 파일을 읽은 후 JPEG 파일 포맷인지 확인
    if( ( fl_fread( pbFileBuffer, 1, dwFileSize, fp ) != dwFileSize ) ||
        ( JPEGInit( pstJpeg, pbFileBuffer, dwFileSize ) == FALSE ) )
    {
        Printf( "[ImageViewer] Read fail or file is not JPEG format %d\n" , dwFileSize);
        FreeMemory( pbFileBuffer );
        FreeMemory( pstJpeg );
        fl_fclose( fp );
        return FALSE;
    }

    // 디코드 결과 출력용 버퍼를 생성
    pstOutputBuffer = AllocateMemory( pstJpeg->width * pstJpeg->height * 
                                       sizeof( COLOR ) );
    // 디코드를 수행한 뒤 정상적으로 처리되었다면 윈도우를 생성
    if( ( pstOutputBuffer != NULL ) &&
        ( JPEGDecode( pstJpeg, pstOutputBuffer ) == TRUE ) )
    {
        // 전체 화면 영역의 크기를 반환
        GetScreenArea( &stScreenArea );
        // 윈도우를 생성, 이미지의 크기와 제목 표시줄의 크기를 고려
        qwWindowID = CreateWindow( ( stScreenArea.iX2 - pstJpeg->width ) / 2, 
                    ( stScreenArea.iY2 - pstJpeg->height ) / 2, pstJpeg->width, 
                    pstJpeg->height + WINDOW_TITLEBAR_HEIGHT, 
                    WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW | WINDOW_FLAGS_RESIZABLE, 
                    pcFileName );
    }
    
    // 윈도우 생성에 실패하거나 출력 버퍼 할당 또는 디코딩에 실패하면 종료
    if( ( qwWindowID == WINDOW_INVALIDID ) || ( pstOutputBuffer == NULL ) )
    {
        Printf( "[ImageViewer] Window create fail or output buffer allocation fail\n" );
        FreeMemory( pbFileBuffer );
        FreeMemory( pstJpeg );
        FreeMemory( pstOutputBuffer );
        DeleteWindow( qwWindowID );
        return FALSE;
    }

    // 윈도우의 너비를 구하여 제목 표시줄 영역을 제외한 나머지 화면 버퍼 영역에 디코딩된
    // 이미지를 복사
    pstWindow = GetWindowWithWindowLock( qwWindowID );
    if( pstWindow != NULL )
    {
        iWindowWidth = GetRectangleWidth( &( pstWindow->stArea ) );
        MemCpy( pstWindow->pstWindowBuffer + ( WINDOW_TITLEBAR_HEIGHT * 
                iWindowWidth ), pstOutputBuffer, pstJpeg->width * 
                pstJpeg->height * sizeof( COLOR ) );

        // 동기화 처리
        Unlock( &( pstWindow->stLock ) );
    }
    
    // 파일 버퍼를 해제하고 윈도우를 화면에 표시
    FreeMemory( pbFileBuffer );
    ShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    //  ESC 키와 윈도우 닫기 버튼을 처리하는 간단한 이벤트 루프
    //--------------------------------------------------------------------------
    // 정상적으로 윈도우를 생성하여 표시했으면 파일 이름 입력 윈도우는 숨김
    ShowWindow( qwMainWindowID, FALSE );
    
    bExit = FALSE;
    while( bExit == FALSE )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( ReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
            Sleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            // ESC 키가 눌리면 그림을 표시하는 윈도우를 삭제하고 파일 이름 입력 윈도우를
            // 표시한 뒤 종료
            if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                DeleteWindow( qwWindowID );
                ShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }                
            break;

            // 윈도우 이벤트 처리
            // 윈도우 크기 변경 이벤트를 처리
        case EVENT_WINDOW_RESIZE:
            // 변경된 윈도우에 디코딩된 이미지를 전송
            BitBlt( qwWindowID, 0, WINDOW_TITLEBAR_HEIGHT, pstOutputBuffer, 
                     pstJpeg->width, pstJpeg->height );
            // 윈도우를 한번 더 표시하여 윈도우 내부에 전송된 이미지를 화면에 업데이트
            ShowWindow( qwWindowID, TRUE );
            break;
            
            // 윈도우 닫기 이벤트를 처리
        case EVENT_WINDOW_CLOSE:
            // 닫기 버튼이 눌리면 이미지 출력 윈도우를 삭제하고 파일 이름 입력 윈도우를
            // 표시한 뒤 종료
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                DeleteWindow( qwWindowID );
                ShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }

    // JPEG 이미지 파일 디코딩에 사용했던 버퍼를 반환
    FreeMemory( pstJpeg );
    FreeMemory( pstOutputBuffer );
    
    fl_fclose( fp );
    return TRUE;
}
