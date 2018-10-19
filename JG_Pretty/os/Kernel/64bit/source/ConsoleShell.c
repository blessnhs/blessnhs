/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   占쎄턀占쎄섭占쎄샹 �뜝�럥嫄쀥뜝�럥�뱺 占쎄슈�뜝�룞�삕占쎌죨�뜝�럥彛� �뜝�럥爰뽩뜝�럥裕� �뜝�럥�냱�뜝�럩逾�
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

// 占쎈슔�걞占쎈젍�뜝�럥援� �뜝�럥占쎈���삕占쎈턄占쎈눀�뜝占� �뜝�럩�젧�뜝�럩踰�
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", Help },
        { "cls", "Clear Screen", Cls },
		{ "urs", "usbreadsector", UsbReadSector },
		{ "uws", "usbwritesector", UsbWriteSector },
		{ "ftp", "Ftp network", Ftp },
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
		{"cat","cat file",Cat},
};

//==============================================================================
//  �뜝�럥堉꾢뜝�럩�젷 �뜝�럥嫄쀥뜝�럩諭� 占쎈쨨占쎈봽�뎽�뜝�럥由��뜝�럥裕� 占쎄턀�겫�뼔援�
//==============================================================================
/**
 *  �뜝�럥嫄쀥뜝�럩踰� 嶺뚮∥�뾼占쎈데 占쎈쇀占쎈쳜�걡占�
 */

void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // 占쎄턀占쎄섭占쎄샹�뜝�럩諭� 占쎄슈�뜝�뜾逾놂옙�뿫由��뜝�럥裕� �뜝�럩�겱占쎈쇀占쎌뒦占쎈윞�댖怨뚰�▼뜝�룞�삕 �뛾�룇瑗뱄옙�꼶
    pstConsoleManager = GetConsoleManager();
    
    // �뜝�럥�뒆�슖�뒫�뿫�뒆�뜝�럥諭� 占쎈퉲占쎈츊占쎌졑
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // 占쎄턀占쎄섭占쎄샹 �뜝�럥嫄� 占쎈꽞占쎄턁筌앾옙 �뜝�럥�깵�뜝�럩�굥�윜諛멥늾�뜝占� TRUE�뤆�룊�삕 �뜝�럥彛� �뜝�럥瑜당뭐癒��뒩�뜝占� �뛾�룇瑗띰옙沅�
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // 占쎄턀占쎄섭占쎄샹 �뜝�럥嫄� 占쎈꽞占쎄턁筌앾옙 �뜝�럥�깵�뜝�럩�굥�윜諛멥늾�뜝占� �뜝�럡�맟�뜝�럩�젧�뜝�럥彛� �뇦猿뗫윪占쎈뮡 占쎈쇀占쎈쳜�걡�뜾紐닷뜝占� 占쎈꽞占쎄턁筌앾옙
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
                // �뜝�럩寃긷뜝�럩�궨 占쎈슔�걠�땻占� �뜝�럩留꾤뇖�궡瑗뜹뜝�룞�삕 �뜝�럥�꽢�뜝�럥�꽑�뜝�럡�맋 �뜝�럥由� 占쎈닱筌뤾쑴�겱 �뜝�럥留곩뜝�럩紐드슖�댙�삕 �뜝�럩逾졾뜝�럥吏쀥뜝�럥由� �뜝�럥堉꾢뜝�럩踰� 占썩뫀踰됧첎�떣�삕占쎈굵 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕
                // 占쎈슔�걞占쎈젍�뜝�럥援� �뵓怨뚯뫓占쎌뱺�뜝�럥�뱺�뜝�럡�맋 嶺뚮씭�쐠�뜝�룞彛뺝뜝占� 占쎈닱筌뤾쑴�겱 �뜝�럡�뀭�뜝�럩�젷
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
                // 占쎈슔�걞占쎈젍�뜝�럥援� �뵓怨뚯뫓占쎌뱺�뜝�럥�뱺 �뜝�럩肉녑뜝�럥裕� 嶺뚮ㅏ援앲�곕뵃�삕占쎈굵 �뜝�럥堉꾢뜝�럥六�
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // �뜝�럥�뒆�슖�뒫�뿫�뒆�뜝�럥諭� 占쎈퉲占쎈츊占쎌졑 �뛾�룊�삕 占쎈슔�걞占쎈젍�뜝�럥援� �뵓怨뚯뫓占쎌뱺 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // �뜝�럥六삣뜝�럥�뒆�뜝�럥諭� �뜝�럡�뀞, CAPS Lock, NUM Lock, Scroll Lock�뜝�룞�삕 占쎈닱占쎈샍占쎈뻣
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB�뜝�룞�삕 占썩뫀踰됧첎�떣�삕占쎈さ�슖�댙�삕 �뜝�럩�쓧�뜝�럩�꼶
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // �뵓怨뚯뫓占쎌뱺�뤆�룊�삕 �뜝�럡�뀣�뜝�럥�닡�뜝�럩肉녑뜝�럩諭� �뜝�럥瑜당춯�엪�삕 �뤆�룊�삕�뜝�럥裕�
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
 *  占쎈슔�걞占쎈젍�뜝�럥援� �뵓怨뚯뫓占쎌뱺�뜝�럥�뱺 �뜝�럩肉녑뜝�럥裕� 占쎈슔�걞占쎈젍�뜝�럥援∽옙紐닷뜝占� 占쎈쑏熬곥룊爰띶뜝�럥由��뜝�럥�뿰 �뜝�럥�돵�뜝�럥堉� 占쎈슔�걞占쎈젍�뜝�럥援∽옙紐닷뜝占� 嶺뚳퐣瑗띰옙遊뷴뜝�럥由��뜝�럥裕� �뜝�럥留쇿뜝�럥�빢占쎈ご�뜝占� �뜝�럥�빢�뜝�럥六�
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // 占썩뫀踰됧첎�떣�삕占쎈さ�슖�댙�삕 占쎈쨨占쎈맪�뗥뜝�럥彛� 占쎈슔�걞占쎈젍�뜝�럥援∽옙紐닷뜝占� 占쎈퉲�겫�슧��
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // 占쎈슔�걞占쎈젍�뜝�럥援� �뜝�럥占쎈���삕占쎈턄占쎈눀�겫�슦諭� �뇦猿볦삕�뜝�럡�뀬�뜝�럥�돵�뜝�럡�맋 �뜝�럥吏쀥뜝�럩逾у뜝�럥由� �뜝�럩逾좑옙逾녘쥈�뫗踰� 占쎈슔�걞占쎈젍�뜝�럥援→뤆�룊�삕 �뜝�럩肉녑뜝�럥裕됬춯�쉻�삕 �뜝�럩�꼪�뜝�럩逾�
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // 占쎈슔�걞占쎈젍�뜝�럥援▼뜝�럩踰� �뼨��梨뤄옙逾졾뜝�룞�삕 �뜝�럡���뜝�럩�뮔�뜝�럩逾� �뜝�럩�걦�뜝�럩�쓧�뜝�럩肉� �뜝�럩逾х뇖�궡瑗백뇡占썲뜝�럥裕됬춯�쉻�삕 �뇦猿볦삕�뜝�럡�뀬
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // 占쎈뎨占쎈봾裕욃뜝�럥諭쒎뜝�럥�뱺�뜝�럡�맋 嶺뚢돦堉싷옙諭� �뜝�럥�빢 �뜝�럥�뵪�뜝�럥堉꾤춯濡녹삕 �뜝�럥�뱺�뜝�럩�몠 占쎈퉲占쎈츊占쎌졑
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� �뜝�럩�겱占쎈쇀占쎌뒦占쎈윞�댖怨뚰�▼뜝�룞�삕 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  占썩뫀踰됧첎�떣�삕占쎈さ�슖�댙�삕 占쎈쨨占쎈맪�뗥뜝�럥彛� �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節륁삕占쎈꺄 �뜝�럡���뜝�럩�뮔占썩뫅�삕 �뼨��梨뤄옙逾좑옙紐닷뜝占� �뛾�룇瑗뱄옙�꼶
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �뜝�럥�맠 �뜝�럩逾졾뜝�럡留� �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節낆쾸�뜝占� �뜝�럥�뵪�뜝�럩紐든춯濡녹삕 �뜝�럡�룎�뤆�룊�삕
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // �뵓怨뚯뫓占쎌뱺�뜝�럩踰� �뼨��梨뤄옙逾좂춯�씭�뒭野껓옙 �뜝�럩逾졾뜝�럥吏쀥뜝�럥由�嶺뚮∥�샍�땻占� 占썩뫀踰됧첎�떣�삕占쎈굵 �뇦猿볦삕�뜝�럡�돰
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� �솻洹ｋ쾴亦낆���삕�뇡占쏙옙�뫅�삕 �뼨��梨뤄옙逾좑옙紐닷뜝占� �뛾�룇瑗뱄옙�꼶
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節륁삕占쎈꺄 �뜝�럩留꾤뇖�궪�삕 �뜝�럥�뵜�뜝�럥�몥�뜝�럩逾졾뜝�럥諭�
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  占쎈슔�걞占쎈젍�뜝�럥援∽옙紐닷뜝占� 嶺뚳퐣瑗띰옙遊뷴뜝�럥由��뜝�럥裕� 占쎄턀�겫�뼔援�
//==============================================================================
/**
 *  �뜝�럥嫄� �뜝�럥利꿨뜝�룞�삕嶺뚮씭흮占쎈굵 占쎈퉲占쎈츊占쎌졑
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

    // �뤆�룊�삕�뜝�럩�궋 �뼨�먯삕 占쎈슔�걞占쎈젍�뜝�럥援▼뜝�럩踰� �뼨��梨뤄옙逾좑옙紐닷뜝占� 占썩몿�뫒亦낉옙
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // �뜝�럥利꿨뜝�룞�삕嶺뚮엪�삕 占쎈퉲占쎈츊占쎌졑
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // 嶺뚮ㅄ維뽨빳�맮�삕占쎈턄 嶺뚮씭�섓옙諭� �뇦猿뗫윪占쎈뮡 �뜝�럡�룎�뜝�럥�뻿�뜝�럡�맋 �솻洹ｋ샍�굢�귣뭄�뜝占�
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
 *  �뜝�럩�꼨嶺뚮∥�샍占쎈굵 嶺뚯쉻�삕�뜝�룞�삕
 */
static void Cls( const char* pcParameterBuffer )
{

    // 嶺뚮엪�삕 �뜝�럩留�繞벿븐뫒�뜝占� �뜝�럥�꺏�뵓怨뚯뫅占쎈룵 �뜝�럩�뮔�뜝�럩紐드슖�댙�삕 �뜝�럡�뀬�뜝�럩�뮔�뜝�럥由�亦껋꼻�삕�슖�댙�삕 �뜝�럩�꼨嶺뚮∥�샍占쎈굵 嶺뚯쉻�삕�뜝�럩�뮧 �뜝�럩�쐩, �뜝�럩逾у뜝�럩逾� 1�슖�댙�삕 占쎈슔�걠�땻占� �뜝�럩逾졾뜝�럥吏�
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // 嶺뚮엪�삕 �뜝�럩留�繞벿븐뫒�뜝占� �뜝�럥�꺏�뵓怨뚯뫅占쎈룵 �뜝�럩�뮔�뜝�럩紐드슖�댙�삕 �뜝�럡�뀬�뜝�럩�뮔�뜝�럥由�亦껋꼻�삕�슖�댙�삕 �뜝�럩�꼨嶺뚮∥�샍占쎈굵 嶺뚯쉻�삕�뜝�럩�뮧 �뜝�럩�쐩, �뜝�럩逾у뜝�럩逾� 1�슖�댙�삕 占쎈슔�걠�땻占� �뜝�럩逾졾뜝�럥吏�
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

static int ftpmain()
{
	startFtpClient("", "");

	return 0;
}

static void Ftp( const char* pcParameterBuffer )
{

	  CreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD , 0, 0,
	                     ( QWORD ) ftpmain, GetAPICID() );
}



static void UsbWriteSector( const char* pcParameterBuffer )
{
	// 嶺뚮엪�삕 �뜝�럩留�繞벿븐뫒�뜝占� �뜝�럥�꺏�뵓怨뚯뫅占쎈룵 �뜝�럩�뮔�뜝�럩紐드슖�댙�삕 �뜝�럡�뀬�뜝�럩�뮔�뜝�럥由�亦껋꼻�삕�슖�댙�삕 �뜝�럩�꼨嶺뚮∥�샍占쎈굵 嶺뚯쉻�삕�뜝�럩�뮧 �뜝�럩�쐩, �뜝�럩逾у뜝�럩逾� 1�슖�댙�삕 占쎈슔�걠�땻占� �뜝�럩逾졾뜝�럥吏�
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
 *  占쎈／�뜝占� 嶺뚮∥��占쎄콫占쎈뎨�뜝占� �뜝�럡苡욜뼨轅명�▼뜝�룞�삕 占쎈퉲占쎈츊占쎌졑
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC占쎈ご�뜝占� �뜝�럩�궨�뜝�럥六삣뜝�럩�굚(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 嶺뚳옙�솒占쏙옙六삣뜝�럥�뱺 �뜝�럥援뜹뜝�럥�꽑�뜝�럩肉녑뜝�럥裕� �뜝�럡���뜝�럩�뮔�뜝�럩諭� �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕슖�댙�삕 �뜝�럩沅η뭐癒뀁삕
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
    // �뜝�럡�뀞�솻洹ｏ옙獄�占� 占쎈슓維귨옙諭쒎슖�뼯�걞占쎌몠占쎈ご�뜝占� �뜝�럥苑겼뜝�럥�돵 PC占쎈ご�뜝占� �뜝�럩�궨�뜝�럥六삣뜝�럩�굚
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋�뜝�럩踰� �뜝�럥爰쀥뜝�럥利뀐옙紐닷뜝占� 嶺뚋뀀룱占쎌젧
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10占쎈／�뜝占� �뜝�럥吏쀥뜝�럥�닱 �솻洹⑥삕�뜝�럩�꼨�뜝�럥由� �뜝�룞�삕�뜝�럩肉� �뜝�럥裕욃뜝�럡�땿�뜝�럥�뒆 �뇖�궠�샍占쎈뮧�뜝�럡�댉占쎈ご�뜝占� �뜝�럩逾졾뜝�럩�뮔�뜝�럥由��뜝�럥�뿰 �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋�뜝�럩踰� �뜝�럥爰쀥뜝�럥利뀐옙紐닷뜝占� �뤆�룄�뫒占쎌젘�뜝�럩�쓤�뜝�럩紐드슖�댙�삕 嶺뚋뀀룱占쎌젧
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // �뜝�룞�삕�뜝�럩逾좑옙�넂�뜝占� �솻洹ｋ쾴占쎈쐸
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC 占쎈슓維귨옙諭쒎슖�뼯�걞占쎌몠�뜝�럥�뱺 �뜝�룞�삕�뜝�럩�궋�뜝�럥彛� �뜝�럩逾у뜝�럩�겱 �뛾�룊�삕 �뜝�럥六삥뤆�룊�삕 �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럥�걫�뜝�럥六�
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC 占쎈슓維귨옙諭쒎슖�뼯�걞占쎌몠�뜝�럥�뱺�뜝�럡�맋 �뜝�럥六삥뤆�룊�삕 �뛾�룊�삕 �뜝�럩逾у뜝�럩�겱占쎈ご�뜝占� �뜝�럩逾��뜝�럩踰�
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럩�뮡�뜝�럡�맖 �뜝�럥�뻹�뜝�럩留꾬옙紐닷뜝占� �솻洹⑥삕�뇦猿볦삕
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럩�뮡�뜝�럡�맖 �뜝�럥�뻹�뜝�럩留꾬옙紐닷뜝占� �솻洹⑥삕�뇦猿볦삕
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
 *  �뜝�럩寃긷뜝�럩�궨 �뜝�럡臾멨뜝�럡�뎽�뜝�럥彛� 嶺뚮ㅄ維�獄�占� �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 占쎈퉲占쎈츊占쎌졑
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
    
    // 占쎄턀�겫�슜�꽑 �뜝�럥�빢嶺뚮씭�뒭野껓옙 占쎈쇀占쎈쳜�걡�뜾紐닷뜝占� �뜝�럥利쇘춯濡ル샍�땻占� �뤆�룊�삕 �뜝�럥裕욑옙�냲�삕繞벿븐뫊占쎌몠�뜝�럥�뱺 �뜝�럩肉녑뜝�럥裕� �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럥�빢占쎈ご�뜝占� �뜝�럥�맠�뜝�럥留�
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // 占쎄턀�겫�슜�꽑�뤆�룊�삕 2�뤆�룊�삕 �뜝�럩逾졾뜝�럡留믣뜝�럩逾좂춯濡녹삕 �뤆�룊�삕 �뜝�럥裕욑옙�냲�삕繞벿븐뫊占쎌몠 �솻洹��뫊餓ο옙 �뤆�룇裕뉛옙�빢占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
    if( iProcessorCount > 1 )
    {
        // �뤆�룊�삕 �뜝�럥裕욑옙�냲�삕繞벿븐뫊占쎌몠 �솻洹��뫊餓ο옙 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뤆�룇裕뉛옙�빢占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럡�뀣�뜝�룞�삕 占썩뫀踰��뚯닂�삕占쎈굵 嶺뚮ㅄ維�筌륅옙 �뜝�럥裕욃뜝�럥�쓡�뜝�럩逾졾뜝�럥裕욅뛾�룆��餓ο옙 嶺뚳옙熬곣뫜�삕
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
        // TCB占쎈ご�뜝占� 占쎈쨨占쎈뿫�돵�뜝�럡�맋 TCB�뤆�룊�삕 �뜝�럡�뀬�뜝�럩�뮔 繞벿살탳占쎈턄嶺뚮〕�삕 ID占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욘뤆�룊�삕 6�뤆�룊�삕 占쎈퉲占쎈츊占쎌졑�뜝�럥彛� �뜝�럥瑜당춯�씭�쐞占쎈펲, 占썩몿�뫒占쎄틭 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럥�걫�뜝�럥六삣뜝�럥留됬춯�쉻�삕 �뜝�럥�뿰占쎄껀�뜝�뜾紐닷뜝占� �뜝�럩�꼪�뜝�럩逾�
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
 *  �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� 占쎈꽞占쎄턁筌앾옙
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� 占쎈꽞占쎄턁筌앾옙
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �뜝�럥諭잌뜝�럩�젧 ID嶺뚮엪�삕 占쎈꽞占쎄턁筌앸툦�삕�뇡占썲뜝�럥裕� �뇦猿뗫윪占쎈뮡
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 �뜝�럥占쎈���삕占쎈츩�뜝�럥諭쒎뜝�럥裕� �뜝�럩�젷�뜝�럩�뇶
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
    // 占쎄턀占쎄섭占쎄샹 �뜝�럥嫄쀯옙�뫅�삕 �뜝�럩占썲뜝�럩紐� �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� �뜝�럩�젷�뜝�럩�뇶�뜝�럥由�占썩뫅�삕 嶺뚮ㅄ維�獄�占� �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡� 占쎈꽞占쎄턁筌앾옙
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 �뜝�럥占쎈���삕占쎈츩�뜝�럥諭쒎뜝�럥裕� �뜝�럡�뀭�뜝�럩�젷 嶺뚮ㅄ維뽨빳�맮�삕�굢�맮�삕�땻占� �뜝�럩�젷�뜝�럩�뇶
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
 *  �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋�뜝�럩踰� �뜝�럡�뀬�뜝�럩�뮔占쎈ご�넫濡�諭� �뜝�럥�걫�뜝�럥六�
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // �뤆�룊�삕 占쎄턀�겫�슜�꽑 �솻洹��뫊餓ο옙 占쎄껀�뜝�룞�삕�뇡占쏙옙紐닷뜝占� 占쎈퉲占쎈츊占쎌졑
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럡�뀣�뜝�룞�삕 占썩뫀踰��뚯닂�삕占쎈굵 嶺뚮ㅄ維�筌륅옙 �뜝�럥裕욃뜝�럥�쓡�뜝�럩逾졾뜝�럥裕욅뛾�룆��餓ο옙 嶺뚳옙熬곣뫜�삕
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// �뜝�럡�뀊�뜝�럥�빢占쎈ご�뜝占� �뛾�룇裕뉑틦占썲뜝�럥六삣뜝�럡�뀞�뼨�먯삕 �뜝�럩留꾢뜝�럥由� �솻洹⑥삕�뜝�럥�빢
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  �뜝�럩肉ュ뜝�럩踰ε뜝�럩踰� �뜝�럡�뀊�뜝�럥�빢占쎈ご�뜝占� �뛾�룇瑗뱄옙�꼶
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  嶺뚳퐣伊숋옙�겱占쎈ご�뜝占� �뜝�럩踰ε뜝�럩�몠�뜝�럡��占쎈뎨占쎈뜆�벟 �뜝�럥由��뜝�럥裕� �뜝�럥裕욃뜝�럩�읉�뜝�럥援�
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // �뜝�럩�궀�뜝�럥六� �뜝�룞�삕�뼨轅명�ч뇡占�
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
 *  �뜝�럥裕욃뜝�럩�읉�뜝�럥援∽옙紐닷뜝占� �뜝�럡臾멨뜝�럡�뎽�뜝�럥由��뜝�럥�뿰 嶺뚮씞�걢占쎈콦占쎈뎨�뜝�룞�삕占쎈츩 �뜝�럩�꼨嶺뚮∥�샍�뜎洹⑥삕占쎌뱿 �솻洹ｋ샍�굢�귦떊占쎈닑占쎈츎 �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럥裕�
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

    // �뜝�럡�뀞�뤆�룊�삕 �뜝�럩肉��뜝�럩�졑�뜝�럥�뵹嶺뚮〕�삕 �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럥裕� 占쎈꽞占쎄턁筌앾옙
    GetCh();
}

/**
 *  嶺뚮씞�걢占쎈콦占쎈뎨�뜝�룞�삕占쎈츩 �뜝�럩�꼨嶺뚮∥�샍占쎈굵 �솻洹ｋ샍�굢�귣뭄�뜝占�
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욘뤆�룊�삕 占쎈꽞占쎄턁筌앾옙 �뜝�럥彛� �뜝�럥瑜당뭐癒��뒩�뜝占� �뜝�룞�삕�뼨�먯삕
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
 *  �뜝�럥吏쀥뜝�럩�쓤 嶺뚮∥��占쎄콫占쎈뎨�뜝占� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럥�걫�뜝�럥六�
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
 *  �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럥�걫�뜝�럥六�
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럩逾��뜝�럩踰�
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // 嶺뚮ㅄ維�占쎌몶 �뵓怨뺣쐡占쎄퉰 占쎈퉲占쎈츊占쎌졑
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // �뜝�럥六삼옙逾놂옙�봾�꽣 �뵓怨뺣쐡占쎄퉰 占쎈퉲占쎈츊占쎌졑
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // �뜝�럥�뿕�뜝�럥援�, �뜝�럥堉꾬옙逾녔�⑤베�맠, �뜝�럥堉꾬옙逾녔�⑤베�맠 �뜝�럥堉� �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // 占쎈／�뜝占� �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢 占쎈퉲占쎈츊占쎌졑
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節놁뿉�뜝占� �뜝�럡�맂�뜝�럥�꽑�뜝�럩沅� LBA �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�럥�뱺�뜝�럡�맋 �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢 嶺뚮씭�뒭野껓옙 �뜝�럩逾��뜝�럩踰�
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
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� 占쎈뎨占쎈봾裕욃뜝�럥諭쒙옙紐닷뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨�뜝�럥由��뜝�럥�뿰 LBA �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�룞�삕 �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢嶺뚮씭�뒭野껓옙 嶺뚮∥��占쎄콫占쎈뎨占쎈맮�삕�뜝占� �뜝�럥留됧뜝�럥堉� �뛾�룇猷뉔뇡占� �뜝�럩逾��뼨�먯삕 �뜝�럥�빢�뜝�럥六�
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뵓怨뚯뫓占쎌뱺�뜝�럩踰� �뜝�럡���뜝�럩�뮔�뜝�럩諭� 占쎈퉲占쎈츊占쎌졑
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

                // 嶺뚮ㅄ維�筌륅옙 �뜝�럥�걗 �뜝�럩�겱占쎈뎨占쎈맩夷� �뜝�럥�걫�뜝�럥六삣뜝�럥由��뜝�럩�졎占썩뫅�삕 16�솻洹ｏ옙占쎈펲 �뜝�럩�굚�뜝�룞�삕 �뇦猿뗫윪占쎈뮡 0�뜝�럩諭� 占쎈퉲�겫�룞�삕�뜝�럥�돵繞벿우삕
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
 *  �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節놁뿉�뜝占� �뜝�럡�맂�뜝�럥�꽑�뜝�럩沅� LBA �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�럥�뱺�뜝�럡�맋 �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢 嶺뚮씭�뒭野껓옙 �뜝�룞�삕
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

    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� 占쎈뎨占쎈봾裕욃뜝�럥諭쒙옙紐닷뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨�뜝�럥由��뜝�럥�뿰 LBA �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�룞�삕 �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢 占쎈퉲�겫�슧��
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
    
    // �뵓怨뚯뫓占쎌뱺占쎈ご�뜝占� �뜝�럥留됧뜝�럥堉� �뛾�룇猷뉔뇡占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� 嶺뚳옙熬곣뫜�삕.
    // �뜝�럥�넮�뜝�럡�돇�뜝�룞�삕 4 �뛾�룆�뾼占쎈턄�뜝�럥諭쒎뜝�럩踰� LBA �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�룞�삕 4 �뛾�룆�뾼占쎈턄�뜝�럥諭쒎뜝�럩踰� �뜝�럥苡븀뼨轅명�쀥뜝占� �뜝�럥�빢�뜝�럥六у뜝�럥彛� �뜝�럩�뮃�뜝�럥�빢�슖�댙�삕 �뜝�럡臾멨뜝�럡�뎽
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // �뜝�럥苡븀뼨�먯삕 �뜝�럥�빢�뜝�럥六�
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뵓怨뚯뫓占쎌뱺�뜝�럩踰� �뜝�럡���뜝�럩�뮔�뜝�럩諭� 占쎈퉲占쎈츊占쎌졑
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

            // 嶺뚮ㅄ維�筌륅옙 �뜝�럥�걗 �뜝�럩�겱占쎈뎨占쎈맩夷� �뜝�럥�걫�뜝�럥六삣뜝�럥由��뜝�럩�졎占썩뫅�삕 16�솻洹ｏ옙占쎈펲 �뜝�럩�굚�뜝�룞�삕 �뇦猿뗫윪占쎈뮡 0�뜝�럩諭� 占쎈퉲�겫�룞�삕�뜝�럥�돵繞벿우삕
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
 *  �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� �뜝�럥�뿼�뇦猿볦삕
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
 *  �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占싸우삕占쎈굵 �뜝�럡臾멨뜝�럡�뎽(�뜝�럥竊쀧춯�엪�삕)
 */
static void FormatHDD( const char* pcParameterBuffer )
{

    disk_t *disk = GetDisk(0);
	if(disk == 0)
	   	return;

	Printf("1.TotalSize %d sector cnt %d\n",disk->size,disk->size/512);
	fl_format(disk->size/512, "JG");

	return ;

	usb_msd_t* msd = disk->data;
	if(msd != 0)
	{
		testMSD(msd);

		Printf("1.TotalSize %d \n",msd->disk.size/512);

		fl_format(msd->disk.size/512, "JG");
	}
	else
	{
		Printf("2.TotalSize %d \n",disk->size/512);
		fl_format(disk->size, "JG");
	}
}

/**
 *  �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럥�걫�뜝�럥六�
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
 *  占쎈쇀占쎈쳜占쎈콦 �뜝�럥�꺏�뜝�럩�쟼�뜝�럡�댉占쎈뎨占쎈봾�뱺 占쎈쑏�뜝占� �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럡臾멨뜝�럡�뎽
 */
static void CreateFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_createdirectory(pcParameterBuffer);
}

static void CreateDirectory( const char* pcParameterBuffer )
{
	char name[256];
	PARAMETERLIST stList;

	// �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  占쎈쇀占쎈쳜占쎈콦 �뜝�럥�꺏�뜝�럩�쟼�뜝�럡�댉占쎈뎨占쎈봾�뱺�뜝�럡�맋 �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럡�뀭�뜝�럩�젷
 */
static void DeleteFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  占쎈쇀占쎈쳜占쎈콦 �뜝�럥�꺏�뜝�럩�쟼�뜝�럡�댉占쎈뎨占쎈봿踰� �뜝�럥�냱�뜝�럩逾� 嶺뚮ㅄ維뽨빳�맮�삕占쎈굵 �뜝�럥�걫�뜝�럥六�
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럡臾멨뜝�럡�뎽�뜝�럥由��뜝�럥�뿰 �뜝�럡�뀞�솻洹ｏ옙獄��쑜�뿉�뜝占� �뜝�럩肉��뜝�럩�졑�뜝�럥彛� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�룞�삕
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� 占쎈뎨占쎈봾裕욃뜝�럥諭쒙옙紐닷뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨�뜝�럥由��뜝�럥�뿰 �뜝�럥�냱�뜝�럩逾� �뜝�럩逾좑옙逾녘쥈�뫗諭� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뜝�럥�냱�뜝�럩逾� �뜝�럡臾멨뜝�럡�뎽
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞�뤆�룊�삕 �뜝�럥�뿼�뜝�럥爰쀥뜝�럩紐드슖�댙�삕 3�뵓怨ㅼ삕 �뜝�럥�빆�뜝�럩�몠嶺뚯쉻�삕 �뜝�럥瑜당뭐癒��뒩�뜝占� �뜝�럡���뜝�럩�뮔�뜝�럩諭� �뜝�럥�냱�뜝�럩逾у뜝�럥�뱺 �뜝�룞�삕
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞�뜝�럩逾좂춯濡녹삕 �뜝�럥�뿼�뜝�럥爰� 3�뵓怨ㅼ삕 �뜝�럥�빆�뜝�럩�몠�댖怨뺤뒧占쎈츎�뤆�룊�삕 �뜝�럩�꼪�뜝�럩逾ε뜝�럥由��뜝�럥�뿰 占쎈쇀占쎈쳜�걡�뜾紐닷뜝占� 占쎈쑏�넫濡�二� �뜝�럡�룎�뤆�룊�삕
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞�뤆�룊�삕 �뜝�럥�닡�뜝�럥鍮띶뜝�럩逾х춯濡녹삕 �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞 �뜝�럩肉��뜝�럩�졑 �뜝�럩�뮃�뜝�럥�빢占쎈ご�뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨
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
 *  �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럥���뜝�럥�꽑�뜝�럡�맋 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럩逾��뜝�럩踰�
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� 占쎈뎨占쎈봾裕욃뜝�럥諭쒙옙紐닷뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨�뜝�럥由��뜝�럥�뿰 �뜝�럥�냱�뜝�럩逾� �뜝�럩逾좑옙逾녘쥈�뫗諭� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뜝�럥�냱�뜝�럩逾� �뜝�럡臾멨뜝�럡�뎽
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�냱�뜝�럩逾у뜝�럩踰� �뜝�럡援�濚밸Ŧ�뒩�뜝占� 占쎈퉲占쎈츊占쎌졑�뜝�럥由��뜝�럥裕� �뇦猿됲뜑占쎈굵 �뛾�룇瑗띰옙沅�
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // 嶺뚮씭�뒩�뜮占� �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞�뜝�럩逾좂춯濡녹삕 �뜝�럥�벀�뜝�럡�댉 �뜝�럡�뀞 �뜝�럩�뮃�뜝�럥�빢占쎈ご�뜝占� 嶺뚯빘鍮볟뜝�룞�삕占쎈뻣�뜝�럡�뀞占썩뫅�삕 20�뜝�럩逾у뜝�럩逾η뭐癒��뒩�뜝占� 占쎈퉲占쎈츊占쎌졑�뜝�럥六ε뜝�럥堉꾤춯濡녹삕
        // �뜝�럥�맠 占쎈퉲占쎈츊占쎌졑�뜝�럥留됬춯�쉻�삕 �뜝�럥�뿰占쎄껀�뜝�뜾紐닷뜝占� 占쎈닱占쎈닔�젆源곴괌�뜝占�
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
 *  �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占싸우삕占쎈꺄 嶺뚳옙�솒占쏙옙六� �뵓怨뚯뫓占쎌뱺�뜝�럥�뱺 �뜝�럩肉녑뜝�럥裕� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� 嶺뚮ㅄ維�筌륅옙 �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �뜝�룞�삕
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
 *  �뜝�럥六삼옙逾놂옙�봾�꽣 �뜝�럥竊쀥뜝�럥諭쒎슖�돦裕녶뜝�룞�삕�땻占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥由��뜝�럥�뿰 �뜝�럥�냱�뜝�럩逾у슖�댙�삕 �뜝�룞�삕�뜝�럩�궋
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
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻占� 占쎈뎨占쎈봾裕욃뜝�럥諭쒙옙紐닷뜝占� 占쎈／占쎈쐝�뵳怨ㅼ삕占쎈꼨�뜝�럥由��뜝�럥�뿰 �뜝�럥�냱�뜝�럩逾� �뜝�럩逾좑옙逾녘쥈�뫗諭� 占쎈퉲�겫�슧��
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
    
    // �뜝�럥六삼옙逾놂옙�봾�꽣 �뜝�럥竊쀥뜝�럥諭쒎뜝�럩踰� FIFO占쎈ご�뜝占� 嶺뚮ㅄ維�筌륅옙 占쎈쑏熬곣뫜�삕
    ClearSerialFIFO();
    
    //==========================================================================
    // �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뼨��梨뤄옙逾졿뤆�룊�삕 �뜝�럥�빢�뜝�럥六욕뜝�럥彛� �뜝�럥瑜당뭐癒��뒩�뜝占� �뼨轅명�∽옙堉꾬옙逾녔�⑤베堉꾢뜝�럥裕� 嶺뚮∥�뾼占쎈뻣嶺뚯쉻�삕占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕, 4 �뛾�룆�뾼占쎈턄�뜝�럥諭쒙옙紐닷뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥由� �뜝�럥痢�
    // Ack占쎈ご�뜝占� �뜝�럩�쓧�뜝�럥苑�
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // �뜝�럡�뀣�뜝�룞�삕 �뜝�럥�빢嶺뚮씭�뒭野껓옙 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뜝�럥�빢�뜝�럥六�
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // �뜝�럥�빢�뜝�럥六욕뜝�럥彛� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뤆�룊�삕 �뜝�럥�뵪�뜝�럥堉꾤춯濡녹삕 �뜝�럩�궀�뜝�럥六� �뜝�룞�삕�뼨�먯삕
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // �뜝�룞�삕�뼨轅명�ч뇡占� �뜝�럥六삥뤆�룄�뫒占쎈턄 30占쎈／�뜝占� �뜝�럩逾졾뜝�럡留믣뜝�럩逾졾뜝�럩逾х춯濡녹삕 Time Out�뜝�럩紐드슖�댙�삕 繞벿살탳�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // 嶺뚮씭�쐠�뜝�룞彛뺧옙留⑼옙紐드슖�댙�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥由� �뜝�럥六삥뤆�룄�뫒占쎈굵 �뤆�룄�돫占쎈뼁
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // �뜝�럩�젧�뜝�럡留믣뜝�럩�쓤�뜝�럩紐드슖�댙�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뼨��梨뤄옙逾좑옙紐닷뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥六ε뜝�럩紐득쾬�꼻�삕�슖�댙�삕, Ack占쎈ご�뜝占� �뜝�럥苑듿뜝�럥六�
    SendSerialData( "A", 1 );

    //==========================================================================
    // �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럡臾멨뜝�럡�뎽�뜝�럥由�占썩뫅�삕 �뜝�럥六삼옙逾놂옙�봾�꽣�슖�돦裕녶뜝�룞�삕�땻占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥由��뜝�럥�뿰 �뜝�럥�냱�뜝�럩逾у뜝�럥�뱺 �뜝�룞�삕�뜝�럩�궋
    //==========================================================================
    // �뜝�럥�냱�뜝�럩逾� �뜝�럡臾멨뜝�럡�뎽
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 �뜝�럥�빢�뜝�럥六�
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // �뵓怨뚯뫓占쎌뱺�뜝�럥�뱺 �뜝�럥堉뽩뜝�럥�닡�뜝�럡�맋 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�룞�삕
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // �뜝�럩逾좂뵓怨뺣쐠�굢占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뤆�룊�삕 �뜝�럥�빢�뜝�럥六욕뜝�럥彛� �뇦猿됲뜑占쎈턄 �뜝�럩肉녑뜝�럥堉꾤춯濡녹삕 ACK �뜝�럩援℡뜝�럥裕� �뜝�럥�냱�뜝�럩逾� �뜝�럥苡븀뼨�먯삕 �뜝�럥�빢�뜝�럥六�
        if( dwTempSize != 0 ) 
        {
            // �뜝�럥�빢�뜝�럥六욕뜝�럥由��뜝�럥裕� 嶺뚯옕�윪�뜝占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뜝�럩踰� 嶺뚮씭�쐠�뜝�룞彛뺧옙留�占쎈뤀嶺뚯쉻�삕 �뜝�럥�빢�뜝�럥六욕뜝�럥六ζㅀ袁㏉�→뤃占� FIFO�뜝�럩踰� �뜝�럡苡욜뼨轅명�ｏ옙逾�
            // 16 �뛾�룆�뾼占쎈턄�뜝�럥諭쒐춯�씭�쐞占쎈펲 �뜝�럥由썹뵓怨뺣쐠�억옙 Ack占쎈ご�뜝占� �뜝�럩�쓧�뜝�럥苑�
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // �뜝�럥苡븀뼨�먯삕 繞벿살탳�굢占� 占쎈닱筌뤾쑴�젷�뤆�룊�삕 �뜝�럡臾며뼨轅명�∽옙�듆 �뛾�룆��餓ο옙 占쎈꽞占쎄턁筌앾옙
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // 嶺뚮씭�쐠�뜝�룞彛뺧옙留⑼옙紐드슖�댙�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럥�빢�뜝�럥六욕뜝�럥由� �뜝�럥六삥뤆�룄�뫒占쎈굵 �뤆�룄�돫占쎈뼁
            qwLastReceivedTickCount = GetTickCount();
        }
        // �뜝�럩逾좂뵓怨뺣쐠�굢占� �뜝�럥�빢�뜝�럥六욕뜝�럥彛� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뤆�룊�삕 �뜝�럥�뵪�뜝�럥堉꾤춯濡녹삕 �뜝�럩�궀�뜝�럥六� �뜝�룞�삕�뼨�먯삕
        else
        {
            Sleep( 0 );
            
            // �뜝�룞�삕�뼨轅명�ч뇡占� �뜝�럥六삥뤆�룄�뫒占쎈턄 10占쎈／�뜝占� �뜝�럩逾졾뜝�럡留믣뜝�럩逾졾뜝�럩逾х춯濡녹삕 Time Out�뜝�럩紐드슖�댙�삕 繞벿살탳�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // �뜝�럩�쓧嶺뚳퐦�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뜝�럩踰� �뜝�럡苡욜뼨轅명�ｅ뜝占� �뜝�럥堉꾢뜝�럩�젷�슖�댙�삕 �뜝�럥�빢�뜝�럥六� �뛾�룇猷뉐뜝占� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뜝�럩踰� �뜝�럡苡욜뼨轅명�▼뜝�룞�삕 占쎈쑏熬곥룊爰띶뜝�럥由��뜝�럥�뿰 �뜝�럡�뎽占썩뫅�삕 �뜝�럥�뿰占쎄껀�뜝�뜾紐닷뜝占�
    // 占쎈퉲占쎈츊占쎌졑�뜝�럥由� �뜝�럥痢�, �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럥堉랃옙�뫅�삕 �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 嶺뚳옙�솒占쏙옙六삼옙紐닷뜝占� 嶺뚮ㅄ維�筌륅옙 占쎈쑏熬곣뫜�삕
    //==========================================================================
    // �뜝�럥�빢�뜝�럥六욕뜝�럥彛� �뼨��梨뤄옙逾좑옙紐닷뜝占� 占쎈쑏熬곥룊爰띶뜝�럥�돵�뜝�럡�맋 占쎈닱筌뤾쑴�젷�뤆�룊�삕 �뛾�룇裕뉑틦占썲뜝�럥六ε뜝�럥裕됬춯�쉻�삕占쎈ご�뜝占� �뜝�럥�걫�뜝�럥六�
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럥堉랃옙�뫅�삕 �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 嶺뚳옙�솒占쏙옙六삼옙紐닷뜝占� �뜝�럡���솻洹ｏ옙繹먲옙
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP �뜝�럡�맟�뜝�럩�젧 �뜝�럥占쎈���삕占쎈턄占쎈눀�뜝占� �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 占쎈퉲占쎈츊占쎌졑
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ�뜝�룞�삕 I/O APIC�뜝�럩踰� �뜝�럩逾ε뜝�럡�댉�뜝�럩�벀�뜝�럥諭� �뜝�럩肉��뜝�럩�졑 �뜝�룞�삕(INTIN)�뜝�럩踰� 占쎄슈�뜝�뜽占썩몿�뫊�뜝�룞�삕 �뜝�룞�삕�뜝�럩�궋�뜝�럥由� �뜝�럥占쎈���삕占쎈턄占쎈눀�겫�슦諭� �뜝�럥�걫�뜝�럥六�
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC占쎈ご�뜝占� 占쎄슈�뜝�뜾逾놂옙�뿫由��뜝�럥裕� �뜝�럩�겱占쎈쇀占쎌뒦占쎈윞�댖怨뚰�ｈ굢占� �뜝�럩肉녑뜝�럥裕� 占쎈퉲占쎈츊占쎌졑 �뜝�럥留쇿뜝�럥�빢占쎈ご�뜝占� �뜝�럩源덌옙鍮듿뜝占�
    PrintIRQToINTINMap();
}

/**
 *  占쎄턀�겫�슜�꽑 �솻洹��뫊餓ο옙 �뜝�럩逾ε뜝�럡�댉�뜝�럩�벀�뜝�럥諭쒙옙紐닷뜝占� 嶺뚳퐣瑗띰옙遊뷴뜝�럥由� �뜝�럩�뮃�뜝�럥�빢占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
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
    
    // MP �뜝�럡�맟�뜝�럩�젧 �뜝�럥占쎈���삕占쎈턄占쎈눀�겫�슜�뱺 �뜝�룞�삕�뜝�럩�궋�뜝�럥彛� 占쎄턀�겫�슜�꽑�뜝�럩踰� �뤆�룇裕뉛옙�빢占쎈ご�뜝占� �뜝�럩逾��뜝�럩踰�
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column 占쎈퉲占쎈츊占쎌졑
    //==========================================================================
    // �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋�뜝�럩踰� �뜝�럥�빢嶺뚮씭�뒭野껓옙 Column�뜝�럩諭� 占쎈퉲占쎈츊占쎌졑
    // �뜝�럥由� 繞벿븐뫒�굢占� 占쎄턀�겫�슜�꽑 4�뤆�룇裕뉔�억옙 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럥由� Column�뜝�럥堉� 15�뇖�궠梨뤄옙諭� �뜝�럥留됧뜝�럥堉ｅ뜝�럥留�
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
        
        // 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럡�뀣�뜝�룞�삕 占썩뫀踰��뚯닂�삕占쎈굵 嶺뚮ㅄ維�筌륅옙 �뜝�럥裕욃뜝�럥�쓡�뜝�럩逾졾뜝�럥裕욃슖�댙�삕 嶺뚳옙熬곣뫜�삕
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row�뜝�룞�삕 �뜝�럩逾ε뜝�럡�댉�뜝�럩�벀�뜝�럥諭� 嶺뚳퐣瑗띰옙遊� �뜝�럩�뮃�뜝�럥�빢 占쎈퉲占쎈츊占쎌졑
    //==========================================================================
    // 占쎈／�뜝占� �뜝�럩逾ε뜝�럡�댉�뜝�럩�벀�뜝�럥諭� �뜝�럩�뮃�뜝�럥�빢�뜝�룞�삕 占쎄턀�겫�슜�꽑 �솻洹⑥삕 �뜝�럩逾ε뜝�럡�댉�뜝�럩�벀�뜝�럥諭� 嶺뚳퐣瑗띰옙遊� �뜝�럩�뮃�뜝�럥�빢占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row占쎈ご�뜝占� 占쎈퉲占쎈츊占쎌졑, �뜝�럥由� 繞벿븐뫒�굢占� 占쎄턀�겫�슜�꽑 4�뤆�룇裕뉔�억옙 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럥由� Column�뜝�럥堉� 15�뇖�궠梨뤄옙諭� �뜝�럥留됧뜝�럥堉�
            if( j == 0 )
            {
                // 20 �뜝�럩逾у뜝�럩逾η춯�씭�쐞占쎈펲 �뜝�럩�꼨嶺뚮〕�삕 �뜝�럩�젧嶺뚯쉻�삕
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
            // 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 �뜝�럡�뀣�뜝�룞�삕 �뜝�럩寃ュ뜝�럥�뿴�뜝�럩諭� 嶺뚮ㅄ維�筌륅옙 �뜝�럥裕욃뜝�럥�쓡�뜝�럩逾졾뜝�럥裕욃슖�댙�삕 嶺뚳옙熬곣뫜�삕
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
 *  �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕뜝�럩踰� �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋 �뇖�궡裕됵옙�꼨�뜝�럥利뀐옙紐닷뜝占� �솻洹⑥삕�뇦猿볦삕
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡� ID �뜝�럥�닡�뜝�럥援� 占쎈퉲�겫�슧��
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �뜝�럥�뒆�슖�돦裕뉓땻占썲뜝�럡�맋 �뇖�궡裕됵옙�꼨�뜝�럥利�(Affinity) 占쎈퉲�겫�슧��
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
 *  VBE 嶺뚮ㅄ維�獄�占� �뜝�럩�젧�솻洹⑥삕 占쎈눀�겫�뼚夷��뜝�럩諭� 占쎈퉲占쎈츊占쎌졑
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE 嶺뚮ㅄ維�獄�占� �뜝�럩�젧�솻洹⑥삕 占쎈눀�겫�뼚夷��뜝�럩諭� �뛾�룇瑗뱄옙�꼶
    pstModeInfo = GetVBEModeInfoBlock();
    
    // �뜝�럥�돵�뜝�럡留믣뜝�럥利꿨뜝�룞�삕 �뜝�럡�돰 �뜝�럩�젧�솻洹ｏ옙�뜝�룞�삕 �뜝�럩留꾦썒�슣�닑餓ο옙 占쎈퉲占쎈츊占쎌졑
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
 *  �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 占쎄턀占쎈츋占쎈굵 �뜝�럥占쎈���삕占쎈츩�뜝�럥諭쒎뜝�럥由��뜝�럥裕� �뜝�럩占썲뜝�룞�삕 �뜝�럩�읉�뵓怨ㅼ삕 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� �뜝�럡臾멨뜝�럡�뎽
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // �뜝�럥吏쀥뜝�럩�쓤 �뜝�럥留됧뜝�럥堉� �뜝�럩寃ュ뜝�럥�뿴�뜝�럥�뱺 4Kbyte 嶺뚮∥��占쎄콫占쎈뎨占쎈맮�삕�뜝占� �뜝�럥留됧뜝�럥堉� �뛾�룇猷뉔뇡占� �뜝�럩占썲뜝�룞�삕 �뜝�럩�읉�뵓怨ㅼ삕 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡울옙紐닷뜝占� �뜝�럡臾멨뜝�럡�뎽�뜝�럥留� 繞벿우삕占쎈쑏熬곣뫅�삕�뜝占� �뜝�럥留�
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // �뜝�럩占썲뜝�룞�삕 �뜝�럩�읉�뵓怨ㅼ삕 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕슖�댙�삕 �뜝�럡�뀬�뜝�럩�뮔�뜝�럥留� SystemCallTestTask() �뜝�럥留쇿뜝�럥�빢�뜝�럩踰� 占쎄턀�겫�뼔援∽옙紐닷뜝占� �뜝�럥留됧뜝�럥堉� �뛾�룇猷뉐뜝占� 嶺뚮∥��占쎄콫占쎈뎨占쎈봾�뱺 �솻洹ｋ쾴亦낉옙
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // �뜝�럩占썲뜝�룞�삕 �뜝�럩�읉�뵓怨ㅼ삕 �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡욕슖�댙�삕 �뜝�럡臾멨뜝�럡�뎽
    CreateTask( TASK_FLAGS_USERLEVEL | TASK_FLAGS_PROCESS,
            pbUserMemory, 0x1000, ( QWORD ) pbUserMemory, TASK_LOADBALANCINGID );
}

static void Cat(const char* pcParameterBuffer )
{
	PARAMETERLIST stList;
	char Name[ 30 ];

	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, Name );

	 FL_FILE *fd = fl_fopen(Name, "r");
	 if(fd == 0)
	 {
		 Printf("fail file open [%s]\n",Name);
		 return ;
	 }

	 Printf("[%s] size %d %d\n",fd->filelength,fd->bytenum);

	 int i;
	 for(i=0;i<fd->filelength;i++)
		 Printf("%c",fl_fgetc(fd));

	 fl_fclose(fd);

}

static void Ping(const char* pcParameterBuffer )
{

	 FL_FILE *fd = fl_fopen("/my.txt", "w");
	 if(fd == 0)
		 return ;


	fl_fwrite("readBuffer", 1,8,fd);
	fl_fclose(fd);

	return;

	PARAMETERLIST stList;
	char ip[30];
 // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
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
 *  �뜝�럩踰볟뜝�럩�뮔�뜝�럥�뒆�슖�돦裕꾬옙�쟽�뜝�럩�궕�뜝�럩諭� �뜝�럥堉꾢뜝�럥六�
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // �뜝�럥�냱�뜝�럩逾ф쾬�꼶梨멱땻節뉖ご�뜝占� 占쎈퉲�겫�슧��
    InitializeParameter( &stList, pcParameterBuffer );
    // �뜝�럩援ⓨ뜝�럥六쇔뜝�럥�뱺 嶺뚮씮�돵�뜝占� �뜝�럥瑜ュ뜝�럩紐든춯濡녹삕 �뜝�럥利꿨뜝�룞�삕嶺뚮씭흮占쎈굵 占쎈퉲占쎈츊占쎌졑�뜝�럥由�占썩뫅�삕 占쎈꽞占쎄턁筌앾옙
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // �뜝�럥�걗 �뵓怨뺣쐠占쎈윯 �뜝�럩逾ε뜝�럩�겱 占쎈닱筌뤾쑴�겱�뜝�럥���뜝�룞�삕 �뜝�럩湲욕뜝�럥占썩댙�삕占쎈さ�슖�댙�삕 嶺뚳퐣瑗띰옙遊�
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // �뜝�럡臾뜹뜝�럥裕욃뜝�럡苡� �뜝�럡臾멨뜝�럡�뎽
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕�뜝�럥�뱺 �뜝�럥援뜹뜝�럥�꽑�뜝�럩肉녑뜝�럥裕� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉占쎈ご�뜝占� �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �솻洹ｋ쾴亦낉옙
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

    // 占쎄껀�뜝�룞�삕占쎈콦 �슖�돦裕놅옙�맠�뤆�룊�삕 �슖�돦裕녽�얇꺈�삕筌랃옙 0x7C05 �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�럥�뱺�뜝�럡�맋 �솻洹ｋ샑占쎄퉰 嶺뚮ㅄ維�獄�占� 占쎈슔�걞�몭硫⑼옙�뫅�삕 IA-32e 嶺뚮ㅄ維�獄�占� 占쎈슔�걞�몭紐뚯삕占쎈굵
    // �뜝�럥占썲뜝�럥由� �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢占쎈ご�뜝占� �뜝�럩逾��뜝�럩踰�
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡� �뜝�럩逾졿쾬�꼶梨룟뜝�룞�삕占쎈츎 0x10000 �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕욃뜝�럥�뱺 �슖�돦裕녽�얇꺈�삕�뵳釉껋쾵�뜝�뜽�뿉�뜝占� �뜝�럩逾좑옙紐닷뜝占� �뼨轅명�ｅ뜝�룞�삕占쎈さ�슖�댙�삕
    // 占쎈슔�걞�몭占� �뜝�럡�돭�뜝�럡�댉 �뜝�럥�빢嶺뚮씭�뒭野껓옙 �뜝�럥�꺏�뜝�럥�꽑嶺뚯쉻�삕 占썩뫀�걙�굢占� �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕 �뜝�럥�뿕�뜝�럥�맠�뤆�룊�삕 �뜝�럩肉녑뜝�럩踰�
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // �뜝�럥六삣윜諛몄굡�몭�꺈�럸占쎄섬�뜝�룞�삕 �뜝�럩�꼪�뜝�럩逾�
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕 �뜝�럡���뜝�럩踰� 嶺뚮ㅄ維�獄�占� �뜝�럥�냱�뜝�럩逾у뜝�럩諭� 嶺뚢돦堉싮뇡�꼻�삕�땻占� �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕뜝�럥�뱺 �솻洹ｋ쾴亦낉옙
    //--------------------------------------------------------------------------
    // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉�뤆�룊�삕 �뜝�럥六삣뜝�럩�굚�뜝�럥由��뜝�럥裕� �뜝�럥�꽑�뜝�럥援▼뜝�럩�읉�뜝�럥裕�
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕 �뜝�럥�뿕�뜝�럥�맠�뜝�럩踰� 嶺뚳퐦�삕 �뵓怨뺣쐠占쎈윯 �뜝�럥�냱�뜝�럩逾� �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉
    pstItem = pstHeader->vstItem;

    // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕�뜝�럥�뱺 �뜝�럥竊쀥뜝�럥留쇿뜝�럥彛� 嶺뚮ㅄ維�獄�占� �뜝�럥�냱�뜝�럩逾у뜝�럩諭� 嶺뚢돦堉싮뇡�꼻�삕�땻占� �솻洹ｋ쾴亦낉옙
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕�뜝�럥�뱺 �뜝�럥竊쀥뜝�럥留쇿뜝�럥彛� �뜝�럥�냱�뜝�럩逾� �뜝�럩逾좑옙逾녘쥈�뫗紐드슖�댙�삕 �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럡臾멨뜝�럡�뎽
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // �뜝�럥�넮�뜝�럡�뀞嶺뚯쉻�삕 �뜝�럥�몥�뜝�럩逾졾뜝�럡�댉 占쎄껀�뜝�띂寃ヨ쥈�뫖�뱺 �뜝�럥竊쀥뜝�럥留쇿뜝�럥彛� �뜝�럥�냱�뜝�럩逾� �뜝�럡���뜝�럩�뮔�뜝�럩諭� �뜝�럥由��뜝�럥援� �뜝�럥�꺏�뜝�럥裕욃뜝�럡苡욕슖�댙�삕 �솻洹ｋ쾴亦낉옙
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럥堉랃옙�뫅�삕 �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 嶺뚳옙�솒占쏙옙六삼옙紐닷뜝占� �뜝�럡���솻洹ｏ옙繹먲옙
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // �뜝�럥�냱�뜝�럩逾у뜝�럩諭� �뜝�럥堉띶뜝�럩踰�
        fl_fclose( fp );

        // �뜝�럥堉꾢뜝�럩踰� �뜝�럥�냱�뜝�럩逾у뜝�럩逾� �뜝�룞�삕�뜝�럩�궋�뜝�럥彛� �뜝�럩留꾤뇖�궡瑗뜸빳占� �뜝�럩逾졾뜝�럥吏�
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // �뜝�럥�냱�뜝�럩逾� �뜝�럥六삣뜝�럥裕욃뜝�럥占쏙옙 嶺뚳옙�솒占쏙옙六삼옙紐닷뜝占� �뜝�럡���솻洹ｏ옙繹먲옙
   // FlushFileSystemCache();
}

