#include "MINTOSLibrary.h"

// 응용프로그램이 보내는 유저 이벤트 타입 정의
#define EVENT_USER_TESTMESSAGE          0x80000001

/**
 *  응용프로그램의 C 언어 엔트리 포인트
 */
int Main( char* pcArgument )
{
    QWORD qwWindowID;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;


    char vcTempBuffer[1024];

    if( IsGraphicMode() == FALSE )
    {        
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        printf( "This task can run only GUI mode~!!!\n" );
        return -1;
    }

    // 윈도우의 크기와 제목 설정
    iWindowWidth = 800;
    iWindowHeight = 600;

     qwWindowID = CreateWindow( 100, 100,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT, vcTempBuffer );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
    	printf( "CreateWindow Failed\n" );
        return -1;
    }

    int idx,iY;
    RECT stButtonArea;
    DIRECTORY *dirlist = (DIRECTORY*)dir("/");
    int count = dirinfilecount("/");
    for(idx = 0;idx<count;idx++)
    {
    	iY = WINDOW_TITLEBAR_HEIGHT + 10;

    	    // 이벤트 정보를 표시하는 영역의 테두리와 윈도우 ID를 표시
    	DrawRect( qwWindowID, 10, iY + 8 + (idx *40), 160, iY + 40+ (idx *40), RGB( 0, 0, 0 ),
    	            TRUE );
    	sprintf( vcTempBuffer, "[%s]", dirlist[idx].filename );
    	DrawText( qwWindowID, 20, iY + 15+ (idx *40), RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),
    	               vcTempBuffer, strlen( vcTempBuffer ) );
    }

    ShowWindow( qwWindowID, TRUE );

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
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_LBUTTONDOWN:
        case EVENT_MOUSE_LBUTTONUP:            
        case EVENT_MOUSE_RBUTTONDOWN:
        case EVENT_MOUSE_RBUTTONUP:
        case EVENT_MOUSE_MBUTTONDOWN:
        case EVENT_MOUSE_MBUTTONUP:
                break;
    
            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
        		break;
    
            // 윈도우 이벤트 처리
        case EVENT_WINDOW_SELECT:
        case EVENT_WINDOW_DESELECT:
        case EVENT_WINDOW_MOVE:
        case EVENT_WINDOW_RESIZE:
        case EVENT_WINDOW_CLOSE:
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                DeleteWindow( qwWindowID );
                return 0;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            // 알 수 없는 이벤트를 출력
            sprintf( vcTempBuffer, "Unknown Event: 0x%X", stReceivedEvent.qwType );
            DrawText( qwWindowID, 20, 20, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
                       vcTempBuffer, strlen( vcTempBuffer ) );

            // 데이터를 출력
            sprintf( vcTempBuffer, "Data0 = 0x%Q, Data1 = 0x%Q, Data2 = 0x%Q",
                      stReceivedEvent.vqwData[ 0 ], stReceivedEvent.vqwData[ 1 ],
                      stReceivedEvent.vqwData[ 2 ] );
            DrawText( qwWindowID, 20,40, RGB( 0, 0, 0 ),
                    WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen( vcTempBuffer ) );
            break;
        }
    
        // 윈도우를 화면에 업데이트
        ShowWindow( qwWindowID, TRUE );
    }

    return 0;
}

#if 0
//------------------------------------------------------------------------------
//  기본 GUI 응용프로그램
//------------------------------------------------------------------------------
/**
 *  기본 GUI 응용프로그램의 코드
 *      GUI 응용프로그램을 만들 때 복사하여 기본 코드로 사용
 */
void BaseGUITask( void )
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;

    //--------------------------------------------------------------------------
    // 그래픽 모드 판단
    //--------------------------------------------------------------------------
    // MINT64 OS가 그래픽 모드로 시작했는지 확인
    if( IsGraphicMode() == FALSE )
    {
        // MINT64 OS가 그래픽 모드로 시작하지 않았다면 실패
        printf( "This task can run only GUI mode~!!!\n" );
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

    // 윈도우 생성 함수 호출, 마우스가 있던 위치를 기준으로 생성
    qwWindowID = CreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT | WINDOW_FLAGS_RESIZABLE,
         "Hello World Window" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }

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
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_LBUTTONDOWN:
        case EVENT_MOUSE_LBUTTONUP:
        case EVENT_MOUSE_RBUTTONDOWN:
        case EVENT_MOUSE_RBUTTONUP:
        case EVENT_MOUSE_MBUTTONDOWN:
        case EVENT_MOUSE_MBUTTONUP:
            // 여기에 마우스 이벤트 처리 코드 넣기
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
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
#endif
