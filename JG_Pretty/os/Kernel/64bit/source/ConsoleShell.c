/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   肄섏넄 �끂�뿉 愿��젴�맂 �냼�뒪 �뙆�씪
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"
#include "Task.h"
#include "Synchronization.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "SerialPort.h"
#include "MPConfigurationTable.h"
#include "LocalAPIC.h"
#include "MultiProcessor.h"
#include "IOAPIC.h"
#include "InterruptHandler.h"
#include "VBE.h"
#include "SystemCall.h"
#include "Loader.h"
#include "storage/usb_msd.h"
#include "RAMDisk.h"
#include "utility.h"
#include "fat32/fat_defs.h"
#include "fat32/fat_filelib.h"
#include "NETPROTOCOL/socket.h"
#include "NETPROTOCOL/if.h"

// 而ㅻ㎤�뱶 �뀒�씠釉� �젙�쓽
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", Help },
        { "cls", "Clear Screen", Cls },
		{ "urs", "usbreadsector", UsbReadSector },
		{ "uws", "usbwritesector", UsbWriteSector },
		{ "tn", "test network", TestNetwork },
	    { "totalram", "Show Total RAM Size", ShowTotalRAMSize },
        { "shutdown", "Shutdown And Reboot OS", Shutdown },
        { "cpuspeed", "Measure Processor Speed", MeasureProcessorSpeed },
        { "date", "Show Date And Time", ShowDateAndTime },
        { "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)",
                ChangeTaskPriority },
        { "tasklist", "Show Task List", ShowTaskList },
        { "killtask", "End Task, ex)killtask 1(ID) or 0xffffffff(All Task)", KillTask },
        { "cpuload", "Show Processor Load", CPULoad },
        { "showmatrix", "Show Matrix Screen", ShowMatrix },
        { "dynamicmeminfo", "Show Dyanmic Memory Information", ShowDyanmicMemoryInformation },
        { "hddinfo", "Show HDD Information", ShowHDDInformation },
        { "readsector", "Read HDD Sector, ex)readsector 0(LBA) 10(count)", 
                ReadSector },
        { "writesector", "Write HDD Sector, ex)writesector 0(LBA) 10(count)", 
                WriteSector },
        { "mounthdd", "Mount HDD", MountHDD },
        { "formathdd", "Format HDD", FormatHDD },
        { "filesysteminfo", "Show File System Information", ShowFileSystemInformation },
        { "createfile", "Create File, ex)createfile a.txt", CreateFileInRootDirectory },
        { "deletefile", "Delete File, ex)deletefile a.txt", DeleteFileInRootDirectory },
        { "dir", "Show Directory", ShowRootDirectory },
        { "writefile", "Write Data To File, ex) writefile a.txt", WriteDataToFile },
        { "readfile", "Read Data From File, ex) readfile a.txt", ReadDataFromFile },
        { "flush", "Flush File System Cache", FlushCache },
        { "download", "Download Data From Serial, ex) download a.txt", DownloadFile },
        { "showmpinfo", "Show MP Configuration Table Information", ShowMPConfigurationTable },
        { "showirqintinmap", "Show IRQ->INITIN Mapping Table", ShowIRQINTINMappingTable },
        { "showintproccount", "Show Interrupt Processing Count", ShowInterruptProcessingCount },
        { "changeaffinity", "Change Task Affinity, ex)changeaffinity 1(ID) 0xFF(Affinity)",
                ChangeTaskAffinity },
        { "vbemodeinfo", "Show VBE Mode Information", ShowVBEModeInfo },
        { "testsystemcall", "Test System Call Operation", TestSystemCall },
        { "exec", "Execute Application Program, ex)exec a.elf argument", 
                ExecuteApplicationProgram },
        { "installpackage", "Install Package To HDD", InstallPackage },
		{"mkdir","make directory",CreateDirectory},
		{"ping","ping test",Ping},
};

//==============================================================================
//  �떎�젣 �끂�쓣 援ъ꽦�븯�뒗 肄붾뱶
//==============================================================================
/**
 *  �끂�쓽 硫붿씤 猷⑦봽
 */
void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // 肄섏넄�쓣 愿�由ы븯�뒗 �옄猷뚭뎄議곕�� 諛섑솚
    pstConsoleManager = GetConsoleManager();
    
    // �봽濡ы봽�듃 異쒕젰
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // 肄섏넄 �끂 醫낅즺 �뵆�옒洹멸� TRUE媛� �맆 �븣源뚯� 諛섎났
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // 肄섏넄 �끂 醫낅즺 �뵆�옒洹멸� �꽕�젙�맂 寃쎌슦 猷⑦봽瑜� 醫낅즺
        if( pstConsoleManager->bExit == TRUE )
        {
            break;
        }
        
        if(  bKey == '`' )
        {
            break;
        }
        else if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // �쁽�옱 而ㅼ꽌 �쐞移섎�� �뼸�뼱�꽌 �븳 臾몄옄 �븵�쑝濡� �씠�룞�븳 �떎�쓬 怨듬갚�쓣 異쒕젰�븯怨�
                // 而ㅻ㎤�뱶 踰꾪띁�뿉�꽌 留덉�留� 臾몄옄 �궘�젣
                GetCursor( &iCursorX, &iCursorY );
                PrintStringXY( iCursorX - 1, iCursorY, " " );
                SetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
        else if( bKey == KEY_ENTER )
        {
            Printf( "\n" );
            
            if( iCommandBufferIndex > 0 )
            {
                // 而ㅻ㎤�뱶 踰꾪띁�뿉 �엳�뒗 紐낅졊�쓣 �떎�뻾
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // �봽濡ы봽�듃 異쒕젰 諛� 而ㅻ㎤�뱶 踰꾪띁 珥덇린�솕
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // �떆�봽�듃 �궎, CAPS Lock, NUM Lock, Scroll Lock�� 臾댁떆
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB�� 怨듬갚�쑝濡� �쟾�솚
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // 踰꾪띁媛� �궓�븘�엳�쓣 �븣留� 媛��뒫
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                Printf( "%c", bKey );
            }
        }



    }


    Printf("Exist Console Shell");

    *( BYTE* ) VBE_STARTGRAPHICMODEFLAGADDRESS = 1;
    StartWindowManager();
}

/*
 *  而ㅻ㎤�뱶 踰꾪띁�뿉 �엳�뒗 而ㅻ㎤�뱶瑜� 鍮꾧탳�븯�뿬 �빐�떦 而ㅻ㎤�뱶瑜� 泥섎━�븯�뒗 �븿�닔瑜� �닔�뻾
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // 怨듬갚�쑝濡� 援щ텇�맂 而ㅻ㎤�뱶瑜� 異붿텧
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // 而ㅻ㎤�뱶 �뀒�씠釉붿쓣 寃��궗�빐�꽌 �룞�씪�븳 �씠由꾩쓽 而ㅻ㎤�뱶媛� �엳�뒗吏� �솗�씤
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // 而ㅻ㎤�뱶�쓽 湲몄씠�� �궡�슜�씠 �셿�쟾�엳 �씪移섑븯�뒗吏� 寃��궗
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // 由ъ뒪�듃�뿉�꽌 李얠쓣 �닔 �뾾�떎硫� �뿉�윭 異쒕젰
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �뙆�씪誘명꽣 �옄猷뚭뎄議곕�� 珥덇린�솕
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  怨듬갚�쑝濡� 援щ텇�맂 �뙆�씪誘명꽣�쓽 �궡�슜怨� 湲몄씠瑜� 諛섑솚
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �뜑 �씠�긽 �뙆�씪誘명꽣媛� �뾾�쑝硫� �굹媛�
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // 踰꾪띁�쓽 湲몄씠留뚰겮 �씠�룞�븯硫댁꽌 怨듬갚�쓣 寃��깋
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �뙆�씪誘명꽣瑜� 蹂듭궗�븯怨� 湲몄씠瑜� 諛섑솚
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �뙆�씪誘명꽣�쓽 �쐞移� �뾽�뜲�씠�듃
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  而ㅻ㎤�뱶瑜� 泥섎━�븯�뒗 肄붾뱶
//==============================================================================
/**
 *  �끂 �룄��留먯쓣 異쒕젰
 */
static void Help( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    
    Printf( "=========================================================\n" );
    Printf( "                    MINT64 Shell Help                    \n" );
    Printf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // 媛��옣 湲� 而ㅻ㎤�뱶�쓽 湲몄씠瑜� 怨꾩궛
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // �룄��留� 異쒕젰
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // 紐⑸줉�씠 留롮쓣 寃쎌슦 �굹�닠�꽌 蹂댁뿬以�
        if( ( i != 0 ) && ( ( i % 20 ) == 0 ) )
        {
            Printf( "Press any key to continue... ('q' is exit) : " );
            if( GetCh() == 'q' )
            {
                Printf( "\n" );
                break;
            }        
            Printf( "\n" );
        }
    }
}

/**
 *  �솕硫댁쓣 吏���
 */
static void Cls( const char* pcParameterBuffer )
{

    // 留� �쐵以꾩� �뵒踰꾧퉭 �슜�쑝濡� �궗�슜�븯誘�濡� �솕硫댁쓣 吏��슫 �썑, �씪�씤 1濡� 而ㅼ꽌 �씠�룞
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // 留� �쐵以꾩� �뵒踰꾧퉭 �슜�쑝濡� �궗�슜�븯誘�濡� �솕硫댁쓣 吏��슫 �썑, �씪�씤 1濡� 而ㅼ꽌 �씠�룞
    //ClearScreen();

	int idx = 1;
    static char buffer[1024];
	//while(1)
	{

		Sleep(10);
	    Printf("UsbReadSector Called usb_msd_count %s\n",pcParameterBuffer);

	    MemSet(buffer,0,1024);

	    disk_t *disk = GetDisk(0);
	    if(disk == 0)
	    	return;

	    Printf("disk 0 Addr = %q\n",disk);
	    Printf("disk 0 Sector Size = %q\n",disk->size);
	    Printf("disk 0 Sector Name = %s\n",disk->port->name);


	    int j = 0;
	    for(j = 0 ; j<10;j++)
	    {
	    	 MemSet(buffer,0,1024);

	    	usb_readSectors(j,buffer,1,disk);
		    Printf("read buff %d\n",buffer[0]);

		    Sleep(10);
	    }

	}
}

static void TestNetwork( const char* pcParameterBuffer )
{
	char hostName[ 30 ];
	memset(hostName,0,sizeof(hostName));
	PARAMETERLIST stList;

	InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, hostName );

	hostent *host;
	sockaddr_in serverSock;
	int sock = 0;

	Printf("hostName %s\n",hostName);

	// Check info about remote computer
	if ((host = gethostbyname ((char *) hostName)) == NULL) {
		Printf ("tftp -> wrong address\n");
		return ;
	}

	int ncsock;
	// Create socket
	if ((ncsock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		Printf ("Cant create socket\n");
		return 0;
	}

	// Fill structure sockaddr_in
	// 1) Family of protocols
	serverSock.sin_family = AF_INET;
	// 2) Number of server port
	serverSock.sin_port = htons (21);
	// 3) Setup ip address of server, where we want to connect
	memcpy (&(serverSock.sin_addr), host->h_addr, host->h_length);

	// Now we are able to connect to remote server
	if (connect (ncsock, (struct sockaddr *) &serverSock, sizeof (serverSock)) == -1) {
		Printf ("Connection cant be estabilished ->\n");
		return ;
	}

	char buf[2048];
	memset(buf,0,2048);
	int ret = recv (ncsock, buf, 499, 0);
	Printf ("recv %s\n",buf);
	memset(buf,0,2048);

	char id[100];
	char pass[100];
	gets_s(id,100);

	char pStr[200];
	SPrintf(pStr, "USER %s\r\n", id);
    send(ncsock, pStr, strlen(pStr),0);

	ret = recv (ncsock, buf, 499, 0);
	Printf ("recv %s\n",buf);
	memset(buf,0,2048);

	gets_s(pass,100);

	SPrintf(pStr, "PASS %s\r\n", pass);
	send(ncsock, pStr, strlen(pStr),0);

	ret = recv (ncsock, buf, 499, 0);
	Printf ("recv %s\n",buf);
	memset(buf,0,2048);

	send(ncsock, "PWD\r\n", 5,0);

	ret = recv (ncsock, buf, 499, 0);
	Printf ("recv %s\n",buf);
	memset(buf,0,2048);


	SPrintf(pStr, "LIST -a\r\n");
	send(ncsock, pStr, strlen(pStr),0);

	ret = recv (ncsock, buf, 499, 0);
	Printf ("recv %s\n",buf);
	memset(buf,0,2048);
}



static void UsbWriteSector( const char* pcParameterBuffer )
{
	// 留� �쐵以꾩� �뵒踰꾧퉭 �슜�쑝濡� �궗�슜�븯誘�濡� �솕硫댁쓣 吏��슫 �썑, �씪�씤 1濡� 而ㅼ꽌 �씠�룞
	    //ClearScreen();


	    Printf("UsbWriteSector Called usb_msd_count %s\n",pcParameterBuffer);
	    static char outbuffer[1024];
	    MemSet(outbuffer,0,1024);

	    disk_t *disk = GetDisk(0);
	   	    if(disk == 0)
	   	    	return;

	    Printf("disk 0 Addr = %q\n",disk);
	    Printf("disk 0 Sector Size = %q\n",disk->size);
	    Printf("disk 0 Sector Name = %s\n",disk->port->name);


	    int k;
	    for(k=0;k<10;k++)
	    {
	    	outbuffer[0] = k;
		    usb_writeSectors(k,outbuffer,1,disk);

		    Printf("buff %s\n",outbuffer);

		    Sleep(10);

	    }
}

/**
 *  珥� 硫붾え由� �겕湲곕�� 異쒕젰
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC瑜� �옱�떆�옉(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // �뙆�씪 �떆�뒪�뀥 罹먯떆�뿉 �뱾�뼱�엳�뒗 �궡�슜�쓣 �븯�뱶 �뵒�뒪�겕濡� �삷源�
    Printf( "Cache Flush... ");
/*    if( FlushFileSystemCache() == TRUE )
    {
        Printf( "Pass\n" );
    }
    else
    {
        Printf( "Fail\n" );
    }
    */
    // �궎蹂대뱶 而⑦듃濡ㅻ윭瑜� �넻�빐 PC瑜� �옱�떆�옉
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  �봽濡쒖꽭�꽌�쓽 �냽�룄瑜� 痢≪젙
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10珥� �룞�븞 蹂��솕�븳 ���엫 �뒪�꺃�봽 移댁슫�꽣瑜� �씠�슜�븯�뿬 �봽濡쒖꽭�꽌�쓽 �냽�룄瑜� 媛꾩젒�쟻�쑝濡� 痢≪젙
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // ���씠癒� 蹂듭썝
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC 而⑦듃濡ㅻ윭�뿉 ���옣�맂 �씪�옄 諛� �떆媛� �젙蹂대�� �몴�떆
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC 而⑦듃濡ㅻ윭�뿉�꽌 �떆媛� 諛� �씪�옄瑜� �씫�쓬
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �깭�뒪�겕�쓽 �슦�꽑 �닚�쐞瑜� 蹂�寃�
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �뙆�씪誘명꽣瑜� 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // �깭�뒪�겕�쓽 �슦�꽑 �닚�쐞瑜� 蹂�寃�
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    bPriority = AToI( vcPriority, 10 );
    
    Printf( "Change Task Priority ID [0x%q] Priority[%d] ", qwID, bPriority );
    if( ChangePriority( qwID, bPriority ) == TRUE )
    {
        Printf( "Success\n" );
    }
    else
    {
        Printf( "Fail\n" );
    }
}

/**
 *  �쁽�옱 �깮�꽦�맂 紐⑤뱺 �깭�뒪�겕�쓽 �젙蹂대�� 異쒕젰
 */
static void ShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    int iTotalTaskCount = 0;
    char vcBuffer[ 20 ];
    int iRemainLength;
    int iProcessorCount;
    
    // 肄붿뼱 �닔留뚰겮 猷⑦봽瑜� �룎硫댁꽌 媛� �뒪耳�以꾨윭�뿉 �엳�뒗 �깭�뒪�겕�쓽 �닔瑜� �뜑�븿
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // 肄붿뼱媛� 2媛� �씠�긽�씠硫� 媛� �뒪耳�以꾨윭 蹂꾨줈 媛쒖닔瑜� 異쒕젰
    if( iProcessorCount > 1 )
    {
        // 媛� �뒪耳�以꾨윭 蹂꾨줈 �깭�뒪�겕�쓽 媛쒖닔瑜� 異쒕젰
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // 異쒕젰�븯怨� �궓�� 怨듦컙�쓣 紐⑤몢 �뒪�럹�씠�뒪諛붾줈 梨꾩�
            iRemainLength = 19 - kStrLen( vcBuffer );
            MemSet( vcBuffer, ' ', iRemainLength );
            vcBuffer[ iRemainLength ] = '\0';
            Printf( vcBuffer );
        }
        
        Printf( "\nPress any key to continue... ('q' is exit) : " );
        if( GetCh() == 'q' )
        {
            Printf( "\n" );
            return ;
        }
        Printf( "\n\n" );
    }
    
    for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        // TCB瑜� 援ы빐�꽌 TCB媛� �궗�슜 以묒씠硫� ID瑜� 異쒕젰
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �깭�뒪�겕媛� 6媛� 異쒕젰�맆 �븣留덈떎, 怨꾩냽 �깭�뒪�겕 �젙蹂대�� �몴�떆�븷吏� �뿬遺�瑜� �솗�씤
            if( ( iCount != 0 ) && ( ( iCount % 6 ) == 0 ) )
            {
                Printf( "Press any key to continue... ('q' is exit) : " );
                if( GetCh() == 'q' )
                {
                    Printf( "\n" );
                    break;
                }
                Printf( "\n" );
            }
            
            Printf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", 1 + iCount++,
                     pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ), 
                     pstTCB->qwFlags, GetListCount( &( pstTCB->stChildThreadList ) ) );
            Printf( "    Core ID[0x%X] CPU Affinity[0x%X]\n", pstTCB->bAPICID,
                     pstTCB->bAffinity );
            Printf( "    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n",
                    pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize );
        }
    }
}

/**
 *  �깭�뒪�겕瑜� 醫낅즺
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �뙆�씪誘명꽣瑜� 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // �깭�뒪�겕瑜� 醫낅즺
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �듅�젙 ID留� 醫낅즺�븯�뒗 寃쎌슦
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �떆�뒪�뀥 �뀒�뒪�듃�뒗 �젣�쇅
        if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
        {
            Printf( "Kill Task ID [0x%q] ", qwID );
            if( EndTask( qwID ) == TRUE )
            {
                Printf( "Success\n" );
            }
            else
            {
                Printf( "Fail\n" );
            }
        }
        else
        {
            Printf( "Task does not exist or task is system task\n" );
        }
    }
    // 肄섏넄 �끂怨� �쑀�쑕 �깭�뒪�겕瑜� �젣�쇅�븯怨� 紐⑤뱺 �깭�뒪�겕 醫낅즺
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �떆�뒪�뀥 �뀒�뒪�듃�뒗 �궘�젣 紐⑸줉�뿉�꽌 �젣�쇅
            if( ( ( qwID >> 32 ) != 0 ) && ( ( pstTCB->qwFlags & TASK_FLAGS_SYSTEM ) == 0x00 ) )
            {
                Printf( "Kill Task ID [0x%q] ", qwID );
                if( EndTask( qwID ) == TRUE )
                {
                    Printf( "Success\n" );
                }
                else
                {
                    Printf( "Fail\n" );
                }
            }
        }
    }
}

/**
 *  �봽濡쒖꽭�꽌�쓽 �궗�슜瑜좎쓣 �몴�떆
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // 媛� 肄붿뼱 蹂꾨줈 遺��븯瑜� 異쒕젰
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // 異쒕젰�븯怨� �궓�� 怨듦컙�쓣 紐⑤몢 �뒪�럹�씠�뒪諛붾줈 梨꾩�
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// �궃�닔瑜� 諛쒖깮�떆�궎湲� �쐞�븳 蹂��닔
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  �엫�쓽�쓽 �궃�닔瑜� 諛섑솚
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  泥좎옄瑜� �쓽�윭�궡由ш쾶 �븯�뒗 �뒪�젅�뱶
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // �옞�떆 ��湲고븿
        Sleep( kRandom() % 20 );
        
        if( ( kRandom() % 20 ) < 16 )
        {
            vcText[ 0 ] = ' ';
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                PrintStringXY( iX, i , vcText );
                Sleep( 50 );
            }
        }        
        else
        {
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                vcText[ 0 ] = ( i + kRandom() ) % 128;
                PrintStringXY( iX, i, vcText );
                Sleep( 50 );
            }
        }
    }
}

/**
 *  �뒪�젅�뱶瑜� �깮�꽦�븯�뿬 留ㅽ듃由��뒪 �솕硫댁쿂�읆 蹂댁뿬二쇰뒗 �봽濡쒖꽭�뒪
 */
static void MatrixProcess( void )
{
    int i;
    
    for( i = 0 ; i < 300 ; i++ )
    {
        if( CreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, 
            ( QWORD ) DropCharactorThread, TASK_LOADBALANCINGID ) == NULL )
        {
            break;
        }
        
        Sleep( kRandom() % 5 + 5 );
    }
    
    Printf( "%d Thread is created\n", i );

    // �궎媛� �엯�젰�릺硫� �봽濡쒖꽭�뒪 醫낅즺
    GetCh();
}

/**
 *  留ㅽ듃由��뒪 �솕硫댁쓣 蹂댁뿬以�
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // �깭�뒪�겕媛� 醫낅즺 �맆 �븣源뚯� ��湲�
        while( ( pstProcess->stLink.qwID >> 32 ) != 0 )
        {
            Sleep( 100 );
        }
    }
    else
    {
        Printf( "Matrix Process Create Fail\n" );
    }
}

/**
 *  �룞�쟻 硫붾え由� �젙蹂대�� �몴�떆
 */
static void ShowDyanmicMemoryInformation( const char* pcParameterBuffer )
{
    QWORD qwStartAddress, qwTotalSize, qwMetaSize, qwUsedSize;
    
    GetDynamicMemoryInformation( &qwStartAddress, &qwTotalSize, &qwMetaSize, 
            &qwUsedSize );

    Printf( "============ Dynamic Memory Information ============\n" );
    Printf( "Start Address: [0x%Q]\n", qwStartAddress );
    Printf( "Total Size:    [0x%Q]byte, [%d]MB\n", qwTotalSize, 
            qwTotalSize / 1024 / 1024 );
    Printf( "Meta Size:     [0x%Q]byte, [%d]KB\n", qwMetaSize, 
            qwMetaSize / 1024 );
    Printf( "Used Size:     [0x%Q]byte, [%d]KB\n", qwUsedSize, qwUsedSize / 1024 );
}

/**
 *  �븯�뱶 �뵒�뒪�겕�쓽 �젙蹂대�� �몴�떆
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // �븯�뱶 �뵒�뒪�겕�쓽 �젙蹂대�� �씫�쓬
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // 紐⑤뜽 踰덊샇 異쒕젰
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // �떆由ъ뼹 踰덊샇 異쒕젰
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // �뿤�뱶, �떎由곕뜑, �떎由곕뜑 �떦 �꽮�꽣 �닔瑜� 異쒕젰
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // 珥� �꽮�꽣 �닔 異쒕젰
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  �븯�뱶 �뵒�뒪�겕�뿉 �뙆�씪誘명꽣濡� �꽆�뼱�삩 LBA �뼱�뱶�젅�뒪�뿉�꽌 �꽮�꽣 �닔 留뚰겮 �씫�쓬
 */
static void ReadSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BYTE bData;
    BOOL bExit = FALSE;
    
    // �뙆�씪誘명꽣 由ъ뒪�듃瑜� 珥덇린�솕�븯�뿬 LBA �뼱�뱶�젅�뒪�� �꽮�꽣 �닔 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // �꽮�꽣 �닔留뚰겮 硫붾え由щ�� �븷�떦 諛쏆븘 �씫湲� �닔�뻾
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // �뜲�씠�꽣 踰꾪띁�쓽 �궡�슜�쓣 異쒕젰
        for( j = 0 ; j < iSectorCount ; j++ )
        {
            for( i = 0 ; i < 512 ; i++ )
            {
                if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
                {
                    Printf( "\nPress any key to continue... ('q' is exit) : " );
                    if( GetCh() == 'q' )
                    {
                        bExit = TRUE;
                        break;
                    }
                }                

                if( ( i % 16 ) == 0 )
                {
                    Printf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
                }

                // 紐⑤몢 �몢 �옄由щ줈 �몴�떆�븯�젮怨� 16蹂대떎 �옉�� 寃쎌슦 0�쓣 異붽��빐以�
                bData = pcBuffer[ j * 512 + i ] & 0xFF;
                if( bData < 16 )
                {
                    Printf( "0" );
                }
                Printf( "%X ", bData );
            }
            
            if( bExit == TRUE )
            {
                break;
            }
        }
        Printf( "\n" );
    }
    else
    {
        Printf( "Read Fail\n" );
    }
    
    FreeMemory( pcBuffer );
}

/**
 *  �븯�뱶 �뵒�뒪�겕�뿉 �뙆�씪誘명꽣濡� �꽆�뼱�삩 LBA �뼱�뱶�젅�뒪�뿉�꽌 �꽮�꽣 �닔 留뚰겮 ��
 */
static void WriteSector( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcLBA[ 50 ], vcSectorCount[ 50 ];
    DWORD dwLBA;
    int iSectorCount;
    char* pcBuffer;
    int i, j;
    BOOL bExit = FALSE;
    BYTE bData;
    static DWORD s_dwWriteCount = 0;

    // �뙆�씪誘명꽣 由ъ뒪�듃瑜� 珥덇린�솕�븯�뿬 LBA �뼱�뱶�젅�뒪�� �꽮�꽣 �닔 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) writesector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );

    s_dwWriteCount++;
    
    // 踰꾪띁瑜� �븷�떦 諛쏆븘 �뜲�씠�꽣瑜� 梨꾩�.
    // �뙣�꽩�� 4 諛붿씠�듃�쓽 LBA �뼱�뱶�젅�뒪�� 4 諛붿씠�듃�쓽 �벐湲곌� �닔�뻾�맂 �슏�닔濡� �깮�꽦
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // �벐湲� �닔�뻾
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // �뜲�씠�꽣 踰꾪띁�쓽 �궡�슜�쓣 異쒕젰
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i++ )
        {
            if( !( ( j == 0 ) && ( i == 0 ) ) && ( ( i % 256 ) == 0 ) )
            {
                Printf( "\nPress any key to continue... ('q' is exit) : " );
                if( GetCh() == 'q' )
                {
                    bExit = TRUE;
                    break;
                }
            }                

            if( ( i % 16 ) == 0 )
            {
                Printf( "\n[LBA:%d, Offset:%d]\t| ", dwLBA + j, i ); 
            }

            // 紐⑤몢 �몢 �옄由щ줈 �몴�떆�븯�젮怨� 16蹂대떎 �옉�� 寃쎌슦 0�쓣 異붽��빐以�
            bData = pcBuffer[ j * 512 + i ] & 0xFF;
            if( bData < 16 )
            {
                Printf( "0" );
            }
            Printf( "%X ", bData );
        }
        
        if( bExit == TRUE )
        {
            break;
        }
    }
    Printf( "\n" );    
    FreeMemory( pcBuffer );    
}

/**
 *  �븯�뱶 �뵒�뒪�겕瑜� �뿰寃�
 */
static void MountHDD( const char* pcParameterBuffer )
{
/*    if( Mount() == FALSE )
    {
        Printf( "HDD Mount Fail\n" );
        return ;
    }
    Printf( "HDD Mount Success\n" );*/
}

/**
 *  �븯�뱶 �뵒�뒪�겕�뿉 �뙆�씪 �떆�뒪�뀥�쓣 �깮�꽦(�룷留�)
 */
static void FormatHDD( const char* pcParameterBuffer )
{

    disk_t *disk = GetDisk(0);
	if(disk == 0)
	   	return;

	Printf("2.TotalSize %q \n",disk->size/512);
			fl_format(disk->size, "JG");

			return ;

	usb_msd_t* msd = disk->data;
	if(msd != 0)
	{
		testMSD(msd);

		Printf("1.TotalSize %q \n",msd->disk.size/512);

		fl_format(msd->disk.size/512, "JG");
	}
	else
	{
		Printf("2.TotalSize %q \n",disk->size/512);
		fl_format(disk->size, "JG");
	}
}

/**
 *  �뙆�씪 �떆�뒪�뀥 �젙蹂대�� �몴�떆
 */
static void ShowFileSystemInformation( const char* pcParameterBuffer )
{
/*    FILESYSTEMMANAGER stManager;
    
    GetFileSystemInformation( &stManager );
    
    Printf( "================== File System Information ==================\n" );
    Printf( "Mouted:\t\t\t\t\t %d\n", stManager.bMounted );
    Printf( "Reserved Sector Count:\t\t\t %d Sector\n", stManager.dwReservedSectorCount );
    Printf( "Cluster Link Table Start Address:\t %d Sector\n", 
            stManager.dwClusterLinkAreaStartAddress );
    Printf( "Cluster Link Table Size:\t\t %d Sector\n", stManager.dwClusterLinkAreaSize );
    Printf( "Data Area Start Address:\t\t %d Sector\n", stManager.dwDataAreaStartAddress );
    Printf( "Total Cluster Count:\t\t\t %d Cluster\n", stManager.dwTotalClusterCount );*/
}

/**
 *  猷⑦듃 �뵒�젆�꽣由ъ뿉 鍮� �뙆�씪�쓣 �깮�꽦
 */
static void CreateFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_createdirectory(pcParameterBuffer);
}

static void CreateDirectory( const char* pcParameterBuffer )
{
	char name[256];
	PARAMETERLIST stList;

	// �뙆�씪誘명꽣瑜� 異붿텧
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  猷⑦듃 �뵒�젆�꽣由ъ뿉�꽌 �뙆�씪�쓣 �궘�젣
 */
static void DeleteFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  猷⑦듃 �뵒�젆�꽣由ъ쓽 �뙆�씪 紐⑸줉�쓣 �몴�떆
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  �뙆�씪�쓣 �깮�꽦�븯�뿬 �궎蹂대뱶濡� �엯�젰�맂 �뜲�씠�꽣瑜� ��
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뙆�씪誘명꽣 由ъ뒪�듃瑜� 珥덇린�솕�븯�뿬 �뙆�씪 �씠由꾩쓣 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뙆�씪 �깮�꽦
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뿏�꽣 �궎媛� �뿰�냽�쑝濡� 3踰� �닃�윭吏� �븣源뚯� �궡�슜�쓣 �뙆�씪�뿉 ��
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // �뿏�꽣 �궎�씠硫� �뿰�냽 3踰� �닃�윭議뚮뒗媛� �솗�씤�븯�뿬 猷⑦봽瑜� 鍮좎졇 �굹媛�
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // �뿏�꽣 �궎媛� �븘�땲�씪硫� �뿏�꽣 �궎 �엯�젰 �슏�닔瑜� 珥덇린�솕
        else
        {
            iEnterCount = 0;
        }
        
        Printf( "%c", bKey );
        if( fl_fwrite( &bKey, 1, 1, fp ) != 1 )
        {
            Printf( "File Wirte Fail\n" );
            break;
        }
    }
    
    Printf( "File Create Success\n" );
    fl_fclose( fp );
}

/**
 *  �뙆�씪�쓣 �뿴�뼱�꽌 �뜲�씠�꽣瑜� �씫�쓬
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뙆�씪誘명꽣 由ъ뒪�듃瑜� 珥덇린�솕�븯�뿬 �뙆�씪 �씠由꾩쓣 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뙆�씪 �깮�꽦
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뙆�씪�쓽 �걹源뚯� 異쒕젰�븯�뒗 寃껋쓣 諛섎났
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // 留뚯빟 �뿏�꽣 �궎�씠硫� �뿏�꽣 �궎 �슏�닔瑜� 利앷��떆�궎怨� 20�씪�씤源뚯� 異쒕젰�뻽�떎硫�
        // �뜑 異쒕젰�븷吏� �뿬遺�瑜� 臾쇱뼱遊�
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            
            if( ( iEnterCount != 0 ) && ( ( iEnterCount % 20 ) == 0 ) )
            {
                Printf( "Press any key to continue... ('q' is exit) : " );
                if( GetCh() == 'q' )
                {
                    Printf( "\n" );
                    break;
                }
                Printf( "\n" );
                iEnterCount = 0;
            }
        }
    }
    fl_fclose( fp );
}

/**
 *  �뙆�씪 �떆�뒪�뀥�쓽 罹먯떆 踰꾪띁�뿉 �엳�뒗 �뜲�씠�꽣瑜� 紐⑤몢 �븯�뱶 �뵒�뒪�겕�뿉 ��
 */
static void FlushCache( const char* pcParameterBuffer )
{
/*    QWORD qwTickCount;
    
    qwTickCount = GetTickCount();
    Printf( "Cache Flush... ");
    if( FlushFileSystemCache() == TRUE )
    {
        Printf( "Pass\n" );
    }
    else
    {
        Printf( "Fail\n" );
    }
    Printf( "Total Time = %d ms\n", GetTickCount() - qwTickCount );*/
}

/**
 *  �떆由ъ뼹 �룷�듃濡쒕��꽣 �뜲�씠�꽣瑜� �닔�떊�븯�뿬 �뙆�씪濡� ���옣
 */
static void DownloadFile( const char* pcParameterBuffer )
{
 /*   PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iFileNameLength;
    DWORD dwDataLength;
    FILE* fp;
    DWORD dwReceivedSize;
    DWORD dwTempSize;
    BYTE vbDataBuffer[ SERIAL_FIFOMAXSIZE ];
    QWORD qwLastReceivedTickCount;
    
    // �뙆�씪誘명꽣 由ъ뒪�듃瑜� 珥덇린�솕�븯�뿬 �뙆�씪 �씠由꾩쓣 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    iFileNameLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iFileNameLength ] = '\0';
    if( ( iFileNameLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || 
        ( iFileNameLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        Printf( "ex)download a.txt\n" );
        return ;
    }
    
    // �떆由ъ뼹 �룷�듃�쓽 FIFO瑜� 紐⑤몢 鍮꾩�
    ClearSerialFIFO();
    
    //==========================================================================
    // �뜲�씠�꽣 湲몄씠媛� �닔�떊�맆 �븣源뚯� 湲곕떎由곕떎�뒗 硫붿떆吏�瑜� 異쒕젰�븯怨�, 4 諛붿씠�듃瑜� �닔�떊�븳 �뮘
    // Ack瑜� �쟾�넚
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // �궓�� �닔留뚰겮 �뜲�씠�꽣 �닔�떊
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // �닔�떊�맂 �뜲�씠�꽣媛� �뾾�떎硫� �옞�떆 ��湲�
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // ��湲고븳 �떆媛꾩씠 30珥� �씠�긽�씠�씪硫� Time Out�쑝濡� 以묒�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // 留덉�留됱쑝濡� �뜲�씠�꽣瑜� �닔�떊�븳 �떆媛꾩쓣 媛깆떊
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // �젙�긽�쟻�쑝濡� �뜲�씠�꽣 湲몄씠瑜� �닔�떊�뻽�쑝誘�濡�, Ack瑜� �넚�떊
    SendSerialData( "A", 1 );

    //==========================================================================
    // �뙆�씪�쓣 �깮�꽦�븯怨� �떆由ъ뼹濡쒕��꽣 �뜲�씠�꽣瑜� �닔�떊�븯�뿬 �뙆�씪�뿉 ���옣
    //==========================================================================
    // �뙆�씪 �깮�꽦
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜲�씠�꽣 �닔�떊
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // 踰꾪띁�뿉 �떞�븘�꽌 �뜲�씠�꽣瑜� ��
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // �씠踰덉뿉 �뜲�씠�꽣媛� �닔�떊�맂 寃껋씠 �엳�떎硫� ACK �삉�뒗 �뙆�씪 �벐湲� �닔�뻾
        if( dwTempSize != 0 ) 
        {
            // �닔�떊�븯�뒗 履쎌� �뜲�씠�꽣�쓽 留덉�留됯퉴吏� �닔�떊�뻽嫄곕굹 FIFO�쓽 �겕湲곗씤
            // 16 諛붿씠�듃留덈떎 �븳踰덉뵫 Ack瑜� �쟾�넚
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // �벐湲� 以묒뿉 臾몄젣媛� �깮湲곕㈃ 諛붾줈 醫낅즺
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // 留덉�留됱쑝濡� �뜲�씠�꽣瑜� �닔�떊�븳 �떆媛꾩쓣 媛깆떊
            qwLastReceivedTickCount = GetTickCount();
        }
        // �씠踰덉뿉 �닔�떊�맂 �뜲�씠�꽣媛� �뾾�떎硫� �옞�떆 ��湲�
        else
        {
            Sleep( 0 );
            
            // ��湲고븳 �떆媛꾩씠 10珥� �씠�긽�씠�씪硫� Time Out�쑝濡� 以묒�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // �쟾泥� �뜲�씠�꽣�쓽 �겕湲곗� �떎�젣濡� �닔�떊 諛쏆� �뜲�씠�꽣�쓽 �겕湲곕�� 鍮꾧탳�븯�뿬 �꽦怨� �뿬遺�瑜�
    // 異쒕젰�븳 �뮘, �뙆�씪�쓣 �떕怨� �뙆�씪 �떆�뒪�뀥 罹먯떆瑜� 紐⑤몢 鍮꾩�
    //==========================================================================
    // �닔�떊�맂 湲몄씠瑜� 鍮꾧탳�빐�꽌 臾몄젣媛� 諛쒖깮�뻽�뒗吏�瑜� �몴�떆
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // �뙆�씪�쓣 �떕怨� �뙆�씪 �떆�뒪�뀥 罹먯떆瑜� �궡蹂대깂
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP �꽕�젙 �뀒�씠釉� �젙蹂대�� 異쒕젰
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ�� I/O APIC�쓽 �씤�꽣�읇�듃 �엯�젰 ��(INTIN)�쓽 愿�怨꾨�� ���옣�븳 �뀒�씠釉붿쓣 �몴�떆
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC瑜� 愿�由ы븯�뒗 �옄猷뚭뎄議곗뿉 �엳�뒗 異쒕젰 �븿�닔瑜� �샇異�
    PrintIRQToINTINMap();
}

/**
 *  肄붿뼱 蹂꾨줈 �씤�꽣�읇�듃瑜� 泥섎━�븳 �슏�닔瑜� 異쒕젰
 */
static void ShowInterruptProcessingCount( const char* pcParameterBuffer )
{
    INTERRUPTMANAGER* pstInterruptManager;
    int i;
    int j;
    int iProcessCount;
    char vcBuffer[ 20 ];
    int iRemainLength;
    int iLineCount;
    
    Printf( "========================== Interrupt Count ==========================\n" );
    
    // MP �꽕�젙 �뀒�씠釉붿뿉 ���옣�맂 肄붿뼱�쓽 媛쒖닔瑜� �씫�쓬
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column 異쒕젰
    //==========================================================================
    // �봽濡쒖꽭�꽌�쓽 �닔留뚰겮 Column�쓣 異쒕젰
    // �븳 以꾩뿉 肄붿뼱 4媛쒖뵫 異쒕젰�븯怨� �븳 Column�떦 15移몄쓣 �븷�떦�븿
    for( i = 0 ; i < iProcessCount ; i++ )
    {
        if( i == 0 )
        {
            Printf( "IRQ Num\t\t" );
        }
        else if( ( i % 4 ) == 0 )
        {
            Printf( "\n       \t\t" );
        }
        SPrintf( vcBuffer, "Core %d", i );
        Printf( vcBuffer );
        
        // 異쒕젰�븯怨� �궓�� 怨듦컙�쓣 紐⑤몢 �뒪�럹�씠�뒪濡� 梨꾩�
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row�� �씤�꽣�읇�듃 泥섎━ �슏�닔 異쒕젰
    //==========================================================================
    // 珥� �씤�꽣�읇�듃 �슏�닔�� 肄붿뼱 蹂� �씤�꽣�읇�듃 泥섎━ �슏�닔瑜� 異쒕젰
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row瑜� 異쒕젰, �븳 以꾩뿉 肄붿뼱 4媛쒖뵫 異쒕젰�븯怨� �븳 Column�떦 15移몄쓣 �븷�떦
            if( j == 0 )
            {
                // 20 �씪�씤留덈떎 �솕硫� �젙吏�
                if( ( iLineCount != 0 ) && ( iLineCount > 10 ) )
                {
                    Printf( "\nPress any key to continue... ('q' is exit) : " );
                    if( GetCh() == 'q' )
                    {
                        Printf( "\n" );
                        return ;
                    }
                    iLineCount = 0;
                    Printf( "\n" );
                }
                Printf( "---------------------------------------------------------------------\n" );
                Printf( "IRQ %d\t\t", i );
                iLineCount += 2;
            }
            else if( ( j % 4 ) == 0 )
            {
                Printf( "\n      \t\t" );
                iLineCount++;
            }
            
            SPrintf( vcBuffer, "0x%Q", pstInterruptManager->vvqwCoreInterruptCount[ j ][ i ] );
            // 異쒕젰�븯怨� �궓�� �쁺�뿭�쓣 紐⑤몢 �뒪�럹�씠�뒪濡� 梨꾩�
            Printf( vcBuffer );
            iRemainLength = 15 - kStrLen( vcBuffer );
            MemSet( vcBuffer, ' ', iRemainLength );
            vcBuffer[ iRemainLength ] = '\0';
            Printf( vcBuffer );
        }
        Printf( "\n" );
    }
}

/**
 *  �깭�뒪�겕�쓽 �봽濡쒖꽭�꽌 移쒗솕�룄瑜� 蹂�寃�
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // �뙆�씪誘명꽣瑜� 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // �깭�뒪�겕 ID �븘�뱶 異붿텧
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �봽濡쒖꽭�꽌 移쒗솕�룄(Affinity) 異붿텧
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        bAffinity = AToI( vcAffinity + 2, 16 );
    }
    else
    {
        bAffinity = AToI( vcAffinity, 10 );
    }
    
    Printf( "Change Task Affinity ID [0x%q] Affinity[0x%x] ", qwID, bAffinity );
    if( ChangeProcessorAffinity( qwID, bAffinity ) == TRUE )
    {
        Printf( "Success\n" );
    }
    else
    {
        Printf( "Fail\n" );
    }
}

/**
 *  VBE 紐⑤뱶 �젙蹂� 釉붾줉�쓣 異쒕젰
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE 紐⑤뱶 �젙蹂� 釉붾줉�쓣 諛섑솚
    pstModeInfo = GetVBEModeInfoBlock();
    
    // �빐�긽�룄�� �깋 �젙蹂대�� �쐞二쇰줈 異쒕젰
    Printf( "VESA %x\n", pstModeInfo->wWinGranulity );
    Printf( "X Resolution: %d\n", pstModeInfo->wXResolution );
    Printf( "Y Resolution: %d\n", pstModeInfo->wYResolution );
    Printf( "Bits Per Pixel: %d\n", pstModeInfo->bBitsPerPixel );
    
    Printf( "Red Mask Size: %d, Field Position: %d\n", pstModeInfo->bRedMaskSize, 
            pstModeInfo->bRedFieldPosition );
    Printf( "Green Mask Size: %d, Field Position: %d\n", pstModeInfo->bGreenMaskSize, 
            pstModeInfo->bGreenFieldPosition );
    Printf( "Blue Mask Size: %d, Field Position: %d\n", pstModeInfo->bBlueMaskSize, 
            pstModeInfo->bBlueFieldPosition );
    Printf( "Physical Base Pointer: 0x%X\n", pstModeInfo->dwPhysicalBasePointer );
    
    Printf( "Linear Red Mask Size: %d, Field Position: %d\n", 
            pstModeInfo->bLinearRedMaskSize, pstModeInfo->bLinearRedFieldPosition );
    Printf( "Linear Green Mask Size: %d, Field Position: %d\n", 
            pstModeInfo->bLinearGreenMaskSize, pstModeInfo->bLinearGreenFieldPosition );
    Printf( "Linear Blue Mask Size: %d, Field Position: %d\n", 
            pstModeInfo->bLinearBlueMaskSize, pstModeInfo->bLinearBlueFieldPosition );
}

/**
 *  �떆�뒪�뀥 肄쒖쓣 �뀒�뒪�듃�븯�뒗 �쑀�� �젅踰� �깭�뒪�겕瑜� �깮�꽦
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // �룞�쟻 �븷�떦 �쁺�뿭�뿉 4Kbyte 硫붾え由щ�� �븷�떦 諛쏆븘 �쑀�� �젅踰� �깭�뒪�겕瑜� �깮�꽦�븷 以�鍮꾨�� �븿
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // �쑀�� �젅踰� �깭�뒪�겕濡� �궗�슜�븷 SystemCallTestTask() �븿�닔�쓽 肄붾뱶瑜� �븷�떦 諛쏆� 硫붾え由ъ뿉 蹂듭궗
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // �쑀�� �젅踰� �깭�뒪�겕濡� �깮�꽦
    CreateTask( TASK_FLAGS_USERLEVEL | TASK_FLAGS_PROCESS,
            pbUserMemory, 0x1000, ( QWORD ) pbUserMemory, TASK_LOADBALANCINGID );
}


static void Ping(const char* pcParameterBuffer )
{

	PARAMETERLIST stList;
	char ip[30];
 // �뙆�씪誘명꽣瑜� 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, ip );

	netif_t *netif = netif_findbyname ("eth0");

		if (!netif) {
			Printf ("ping -> network interface does not exists\n");
			return ;
		}

		unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned char d;

		unsigned g = 0;
		unsigned i = 0;
		unsigned y = strlen (ip);

		if (!y) {
			Printf ("ping -> wrong syntax, please specify address\n");
			return ;
		}

		unsigned h[4];

		while (i < y) {
			if (ip[i] == '.') {
				ip[i] = '\0';
				h[g] = i+1;
				g ++;
			}

			i ++;
		}

		net_ipv4 target = 0;

		if (g != 3) {

			if (dns_cache_get (ip, &target, 4) != 1)
				if (dns_send_request (ip, &target, 4) != 1) {
					Printf ("ping -> bad hostname or ip address format, example: 192.168.1.1\n");
					return ;
				}
		} else {
			a = atoi (ip);
			b = atoi (ip+h[0]);
			c = atoi (ip+h[1]);
			d = atoi (ip+h[2]);

			target = NET_IPV4_TO_ADDR (a, b, c, d);
		}

		char address[20];
		net_proto_ip_convert2 (target, address);

		i = 0;

		while (i < 8) {
			unsigned long time_a = GetTickCount();
			unsigned ret = net_proto_icmp_ping (netif, target);
			if (ret) {
				Printf ("ping -> %s (%s) - %ums\n", ip, address, (GetTickCount() - time_a));
			} else {
				Printf ("ping -> Host %s is unreachable\n", ip);
				break;
			}

			i ++;
		}

		return 1;
}

/**
 *  �쓳�슜�봽濡쒓렇�옩�쓣 �떎�뻾
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // �뙆�씪誘명꽣瑜� 異붿텧
    InitializeParameter( &stList, pcParameterBuffer );
    // �삎�떇�뿉 留욎� �븡�쑝硫� �룄��留먯쓣 異쒕젰�븯怨� 醫낅즺
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // �몢 踰덉㎏ �씤�옄 臾몄옄�뿴�� �샃�뀡�쑝濡� 泥섎━
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // �깭�뒪�겕 �깮�꽦
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  �뙣�궎吏��뿉 �뱾�뼱�엳�뒗 �뜲�씠�꽣瑜� �븯�뱶 �뵒�뒪�겕�뿉 蹂듭궗
 */
static void InstallPackage( const char* pcParameterBuffer )
{
    PACKAGEHEADER* pstHeader;
    PACKAGEITEM* pstItem;
    WORD wKernelTotalSectorCount;
    int i;
    FL_FILE* fp;
    QWORD qwDataAddress;

    Printf( "Package Install Start...\n" );

    // 遺��듃 濡쒕뜑媛� 濡쒕뵫�맂 0x7C05 �뼱�뱶�젅�뒪�뿉�꽌 蹂댄샇 紐⑤뱶 而ㅻ꼸怨� IA-32e 紐⑤뱶 而ㅻ꼸�쓣
    // �빀�븳 �꽮�꽣 �닔瑜� �씫�쓬
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // �뵒�뒪�겕 �씠誘몄��뒗 0x10000 �뼱�뱶�젅�뒪�뿉 濡쒕뵫�릺誘�濡� �씠瑜� 湲곗��쑝濡�
    // 而ㅻ꼸 �꽮�꽣 �닔留뚰겮 �뼥�뼱吏� 怨녹뿉 �뙣�궎吏� �뿤�뜑媛� �엳�쓬
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // �떆洹몃꼫泥섎�� �솗�씤
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // �뙣�궎吏� �궡�쓽 紐⑤뱺 �뙆�씪�쓣 李얠븘�꽌 �븯�뱶 �뵒�뒪�겕�뿉 蹂듭궗
    //--------------------------------------------------------------------------
    // �뙣�궎吏� �뜲�씠�꽣媛� �떆�옉�븯�뒗 �뼱�뱶�젅�뒪
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // �뙣�궎吏� �뿤�뜑�쓽 泥� 踰덉㎏ �뙆�씪 �뜲�씠�꽣
    pstItem = pstHeader->vstItem;

    // �뙣�궎吏��뿉 �룷�븿�맂 紐⑤뱺 �뙆�씪�쓣 李얠븘�꽌 蹂듭궗
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // �뙣�궎吏��뿉 �룷�븿�맂 �뙆�씪 �씠由꾩쑝濡� �뙆�씪�쓣 �깮�꽦
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // �뙣�궎吏� �뜲�씠�꽣 遺�遺꾩뿉 �룷�븿�맂 �뙆�씪 �궡�슜�쓣 �븯�뱶 �뵒�뒪�겕濡� 蹂듭궗
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // �뙆�씪�쓣 �떕怨� �뙆�씪 �떆�뒪�뀥 罹먯떆瑜� �궡蹂대깂
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // �뙆�씪�쓣 �떕�쓬
        fl_fclose( fp );

        // �떎�쓬 �뙆�씪�씠 ���옣�맂 �쐞移섎줈 �씠�룞
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // �뙆�씪 �떆�뒪�뀥 罹먯떆瑜� �궡蹂대깂
   // FlushFileSystemCache();
}

