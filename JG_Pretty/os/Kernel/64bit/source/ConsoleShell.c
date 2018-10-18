/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �굜�꼷�꼧 占쎈걗占쎈퓠 �꽴占쏙옙�졃占쎈쭆 占쎈꺖占쎈뮞 占쎈솁占쎌뵬
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

// �뚣끇�렎占쎈굡 占쎈�믭옙�뵠�뇡占� 占쎌젟占쎌벥
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
//  占쎈뼄占쎌젫 占쎈걗占쎌뱽 �뤃�딄쉐占쎈릭占쎈뮉 �굜遺얜굡
//==============================================================================
/**
 *  占쎈걗占쎌벥 筌롫뗄�뵥 �뙴�뫂遊�
 */

void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // �굜�꼷�꼧占쎌뱽 �꽴占썹뵳�뗫릭占쎈뮉 占쎌쁽�뙴�슡�럡鈺곌퀡占쏙옙 獄쏆꼹�넎
    pstConsoleManager = GetConsoleManager();
    
    // 占쎈늄嚥⊥뗫늄占쎈뱜 �빊�뮆�젾
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // �굜�꼷�꼧 占쎈걗 �넫�굝利� 占쎈탣占쎌삋域밸㈇占� TRUE揶쏉옙 占쎈쭍 占쎈르繹먮슣占� 獄쏆꼶�궗
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // �굜�꼷�꼧 占쎈걗 �넫�굝利� 占쎈탣占쎌삋域밸㈇占� 占쎄퐬占쎌젟占쎈쭆 野껋럩�뒭 �뙴�뫂遊썹몴占� �넫�굝利�
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
                // 占쎌겱占쎌삺 �뚣끉苑� 占쎌맄燁살꼶占쏙옙 占쎈섯占쎈선占쎄퐣 占쎈립 �눧紐꾩쁽 占쎈링占쎌몵嚥∽옙 占쎌뵠占쎈짗占쎈립 占쎈뼄占쎌벉 �⑤벉媛싷옙�뱽 �빊�뮆�젾占쎈릭�⑨옙
                // �뚣끇�렎占쎈굡 甕곌쑵�쓠占쎈퓠占쎄퐣 筌띾뜆占쏙쭕占� �눧紐꾩쁽 占쎄텣占쎌젫
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
                // �뚣끇�렎占쎈굡 甕곌쑵�쓠占쎈퓠 占쎌뿳占쎈뮉 筌뤿굝議딉옙�뱽 占쎈뼄占쎈뻬
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // 占쎈늄嚥⊥뗫늄占쎈뱜 �빊�뮆�젾 獄쏉옙 �뚣끇�렎占쎈굡 甕곌쑵�쓠 �룯�뜃由곤옙�넅
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // 占쎈뻻占쎈늄占쎈뱜 占쎄텕, CAPS Lock, NUM Lock, Scroll Lock占쏙옙 �눧�똻�뻻
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB占쏙옙 �⑤벉媛싷옙�몵嚥∽옙 占쎌읈占쎌넎
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // 甕곌쑵�쓠揶쏉옙 占쎄텚占쎈툡占쎌뿳占쎌뱽 占쎈르筌랃옙 揶쏉옙占쎈뮟
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
 *  �뚣끇�렎占쎈굡 甕곌쑵�쓠占쎈퓠 占쎌뿳占쎈뮉 �뚣끇�렎占쎈굡�몴占� �뜮袁㏉꺍占쎈릭占쎈연 占쎈퉸占쎈뼣 �뚣끇�렎占쎈굡�몴占� 筌ｌ꼶�봺占쎈릭占쎈뮉 占쎈맙占쎈땾�몴占� 占쎈땾占쎈뻬
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �⑤벉媛싷옙�몵嚥∽옙 �뤃�됲뀋占쎈쭆 �뚣끇�렎占쎈굡�몴占� �빊遺욱뀱
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // �뚣끇�렎占쎈굡 占쎈�믭옙�뵠�뇡遺우뱽 野껓옙占쎄텢占쎈퉸占쎄퐣 占쎈짗占쎌뵬占쎈립 占쎌뵠�뵳袁⑹벥 �뚣끇�렎占쎈굡揶쏉옙 占쎌뿳占쎈뮉筌욑옙 占쎌넇占쎌뵥
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // �뚣끇�렎占쎈굡占쎌벥 疫뀀챷�뵠占쏙옙 占쎄땀占쎌뒠占쎌뵠 占쎌끏占쎌읈占쎌뿳 占쎌뵬燁살꼹釉�占쎈뮉筌욑옙 野껓옙占쎄텢
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // �뵳�딅뮞占쎈뱜占쎈퓠占쎄퐣 筌≪뼚�뱽 占쎈땾 占쎈씨占쎈뼄筌롳옙 占쎈퓠占쎌쑎 �빊�뮆�젾
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  占쎈솁占쎌뵬沃섎챸苑� 占쎌쁽�뙴�슡�럡鈺곌퀡占쏙옙 �룯�뜃由곤옙�넅
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �⑤벉媛싷옙�몵嚥∽옙 �뤃�됲뀋占쎈쭆 占쎈솁占쎌뵬沃섎챸苑ｏ옙�벥 占쎄땀占쎌뒠�⑨옙 疫뀀챷�뵠�몴占� 獄쏆꼹�넎
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // 占쎈쐭 占쎌뵠占쎄맒 占쎈솁占쎌뵬沃섎챸苑ｅ첎占� 占쎈씨占쎌몵筌롳옙 占쎄돌揶쏉옙
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // 甕곌쑵�쓠占쎌벥 疫뀀챷�뵠筌띾슦寃� 占쎌뵠占쎈짗占쎈릭筌롫똻苑� �⑤벉媛싷옙�뱽 野껓옙占쎄퉳
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� 癰귣벊沅쀯옙釉��⑨옙 疫뀀챷�뵠�몴占� 獄쏆꼹�넎
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // 占쎈솁占쎌뵬沃섎챸苑ｏ옙�벥 占쎌맄燁삼옙 占쎈씜占쎈쑓占쎌뵠占쎈뱜
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  �뚣끇�렎占쎈굡�몴占� 筌ｌ꼶�봺占쎈릭占쎈뮉 �굜遺얜굡
//==============================================================================
/**
 *  占쎈걗 占쎈즲占쏙옙筌띾Ŋ�뱽 �빊�뮆�젾
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

    // 揶쏉옙占쎌삢 疫뀐옙 �뚣끇�렎占쎈굡占쎌벥 疫뀀챷�뵠�몴占� �④쑴沅�
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // 占쎈즲占쏙옙筌랃옙 �빊�뮆�젾
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // 筌뤴뫖以됵옙�뵠 筌띾‘�뱽 野껋럩�뒭 占쎄돌占쎈떊占쎄퐣 癰귣똻肉т빳占�
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
 *  占쎌넅筌롫똻�뱽 筌욑옙占쏙옙
 */
static void Cls( const char* pcParameterBuffer )
{

    // 筌랃옙 占쎌맮餓κ쑴占� 占쎈탵甕곌쑨�돪 占쎌뒠占쎌몵嚥∽옙 占쎄텢占쎌뒠占쎈릭沃섓옙嚥∽옙 占쎌넅筌롫똻�뱽 筌욑옙占쎌뒲 占쎌뜎, 占쎌뵬占쎌뵥 1嚥∽옙 �뚣끉苑� 占쎌뵠占쎈짗
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // 筌랃옙 占쎌맮餓κ쑴占� 占쎈탵甕곌쑨�돪 占쎌뒠占쎌몵嚥∽옙 占쎄텢占쎌뒠占쎈릭沃섓옙嚥∽옙 占쎌넅筌롫똻�뱽 筌욑옙占쎌뒲 占쎌뜎, 占쎌뵬占쎌뵥 1嚥∽옙 �뚣끉苑� 占쎌뵠占쎈짗
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
	// 筌랃옙 占쎌맮餓κ쑴占� 占쎈탵甕곌쑨�돪 占쎌뒠占쎌몵嚥∽옙 占쎄텢占쎌뒠占쎈릭沃섓옙嚥∽옙 占쎌넅筌롫똻�뱽 筌욑옙占쎌뒲 占쎌뜎, 占쎌뵬占쎌뵥 1嚥∽옙 �뚣끉苑� 占쎌뵠占쎈짗
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
 *  �룯占� 筌롫뗀�걟�뵳占� 占쎄쾿疫꿸퀡占쏙옙 �빊�뮆�젾
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC�몴占� 占쎌삺占쎈뻻占쎌삂(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 筌�癒��뻻占쎈퓠 占쎈굶占쎈선占쎌뿳占쎈뮉 占쎄땀占쎌뒠占쎌뱽 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿嚥∽옙 占쎌궥繹먲옙
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
    // 占쎄텕癰귣�諭� �뚢뫂�뱜嚥▲끇�쑎�몴占� 占쎈꽰占쎈퉸 PC�몴占� 占쎌삺占쎈뻻占쎌삂
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  占쎈늄嚥≪뮇苑�占쎄퐣占쎌벥 占쎈꺗占쎈즲�몴占� 筌β돦�젟
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10�룯占� 占쎈짗占쎈툧 癰귨옙占쎌넅占쎈립 占쏙옙占쎌뿫 占쎈뮞占쎄틕占쎈늄 燁삳똻�뒲占쎄숲�몴占� 占쎌뵠占쎌뒠占쎈릭占쎈연 占쎈늄嚥≪뮇苑�占쎄퐣占쎌벥 占쎈꺗占쎈즲�몴占� 揶쏄쑴�젔占쎌읅占쎌몵嚥∽옙 筌β돦�젟
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // 占쏙옙占쎌뵠�솒占� 癰귣벊�뜚
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC �뚢뫂�뱜嚥▲끇�쑎占쎈퓠 占쏙옙占쎌삢占쎈쭆 占쎌뵬占쎌쁽 獄쏉옙 占쎈뻻揶쏉옙 占쎌젟癰귣�占쏙옙 占쎈ご占쎈뻻
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC �뚢뫂�뱜嚥▲끇�쑎占쎈퓠占쎄퐣 占쎈뻻揶쏉옙 獄쏉옙 占쎌뵬占쎌쁽�몴占� 占쎌뵭占쎌벉
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  占쎄묶占쎈뮞占쎄쾿占쎌벥 占쎌뒭占쎄퐨 占쎈떄占쎌맄�몴占� 癰귨옙野껓옙
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // 占쎄묶占쎈뮞占쎄쾿占쎌벥 占쎌뒭占쎄퐨 占쎈떄占쎌맄�몴占� 癰귨옙野껓옙
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
 *  占쎌겱占쎌삺 占쎄문占쎄쉐占쎈쭆 筌뤴뫀諭� 占쎄묶占쎈뮞占쎄쾿占쎌벥 占쎌젟癰귣�占쏙옙 �빊�뮆�젾
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
    
    // �굜遺용선 占쎈땾筌띾슦寃� �뙴�뫂遊썹몴占� 占쎈즼筌롫똻苑� 揶쏉옙 占쎈뮞�놂옙餓κ쑬�쑎占쎈퓠 占쎌뿳占쎈뮉 占쎄묶占쎈뮞占쎄쾿占쎌벥 占쎈땾�몴占� 占쎈쐭占쎈맙
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // �굜遺용선揶쏉옙 2揶쏉옙 占쎌뵠占쎄맒占쎌뵠筌롳옙 揶쏉옙 占쎈뮞�놂옙餓κ쑬�쑎 癰귢쑬以� 揶쏆뮇�땾�몴占� �빊�뮆�젾
    if( iProcessorCount > 1 )
    {
        // 揶쏉옙 占쎈뮞�놂옙餓κ쑬�쑎 癰귢쑬以� 占쎄묶占쎈뮞占쎄쾿占쎌벥 揶쏆뮇�땾�몴占� �빊�뮆�젾
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // �빊�뮆�젾占쎈릭�⑨옙 占쎄텚占쏙옙 �⑤벀而숋옙�뱽 筌뤴뫀紐� 占쎈뮞占쎈읂占쎌뵠占쎈뮞獄쏅뗀以� 筌�袁⑼옙
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
        // TCB�몴占� �뤃�뗫퉸占쎄퐣 TCB揶쏉옙 占쎄텢占쎌뒠 餓λ쵐�뵠筌롳옙 ID�몴占� �빊�뮆�젾
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // 占쎄묶占쎈뮞占쎄쾿揶쏉옙 6揶쏉옙 �빊�뮆�젾占쎈쭍 占쎈르筌띾뜄�뼄, �④쑴�꺗 占쎄묶占쎈뮞占쎄쾿 占쎌젟癰귣�占쏙옙 占쎈ご占쎈뻻占쎈막筌욑옙 占쎈연�겫占썹몴占� 占쎌넇占쎌뵥
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
 *  占쎄묶占쎈뮞占쎄쾿�몴占� �넫�굝利�
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // 占쎄묶占쎈뮞占쎄쾿�몴占� �넫�굝利�
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 占쎈뱟占쎌젟 ID筌랃옙 �넫�굝利븝옙釉�占쎈뮉 野껋럩�뒭
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // 占쎈뻻占쎈뮞占쎈�� 占쎈�믭옙�뮞占쎈뱜占쎈뮉 占쎌젫占쎌뇚
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
    // �굜�꼷�꼧 占쎈걗�⑨옙 占쎌�占쎌몧 占쎄묶占쎈뮞占쎄쾿�몴占� 占쎌젫占쎌뇚占쎈릭�⑨옙 筌뤴뫀諭� 占쎄묶占쎈뮞占쎄쾿 �넫�굝利�
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // 占쎈뻻占쎈뮞占쎈�� 占쎈�믭옙�뮞占쎈뱜占쎈뮉 占쎄텣占쎌젫 筌뤴뫖以됵옙肉됵옙苑� 占쎌젫占쎌뇚
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
 *  占쎈늄嚥≪뮇苑�占쎄퐣占쎌벥 占쎄텢占쎌뒠�몴醫롮뱽 占쎈ご占쎈뻻
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // 揶쏉옙 �굜遺용선 癰귢쑬以� �겫占쏙옙釉��몴占� �빊�뮆�젾
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // �빊�뮆�젾占쎈릭�⑨옙 占쎄텚占쏙옙 �⑤벀而숋옙�뱽 筌뤴뫀紐� 占쎈뮞占쎈읂占쎌뵠占쎈뮞獄쏅뗀以� 筌�袁⑼옙
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// 占쎄텆占쎈땾�몴占� 獄쏆뮇源�占쎈뻻占쎄텕疫뀐옙 占쎌맄占쎈립 癰귨옙占쎈땾
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  占쎌뿫占쎌벥占쎌벥 占쎄텆占쎈땾�몴占� 獄쏆꼹�넎
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  筌ｌ쥙�쁽�몴占� 占쎌벥占쎌쑎占쎄땀�뵳�덉쓺 占쎈릭占쎈뮉 占쎈뮞占쎌쟿占쎈굡
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // 占쎌삛占쎈뻻 占쏙옙疫꿸퀬釉�
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
 *  占쎈뮞占쎌쟿占쎈굡�몴占� 占쎄문占쎄쉐占쎈릭占쎈연 筌띲끋�뱜�뵳占쏙옙�뮞 占쎌넅筌롫똻荑귨옙�쓥 癰귣똻肉т틠�눖�뮉 占쎈늄嚥≪뮇苑�占쎈뮞
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

    // 占쎄텕揶쏉옙 占쎌뿯占쎌젾占쎈┷筌롳옙 占쎈늄嚥≪뮇苑�占쎈뮞 �넫�굝利�
    GetCh();
}

/**
 *  筌띲끋�뱜�뵳占쏙옙�뮞 占쎌넅筌롫똻�뱽 癰귣똻肉т빳占�
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // 占쎄묶占쎈뮞占쎄쾿揶쏉옙 �넫�굝利� 占쎈쭍 占쎈르繹먮슣占� 占쏙옙疫뀐옙
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
 *  占쎈짗占쎌읅 筌롫뗀�걟�뵳占� 占쎌젟癰귣�占쏙옙 占쎈ご占쎈뻻
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
 *  占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎌벥 占쎌젟癰귣�占쏙옙 占쎈ご占쎈뻻
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎌벥 占쎌젟癰귣�占쏙옙 占쎌뵭占쎌벉
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // 筌뤴뫀�쑞 甕곕뜇�깈 �빊�뮆�젾
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // 占쎈뻻�뵳�딅섰 甕곕뜇�깈 �빊�뮆�젾
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // 占쎈엘占쎈굡, 占쎈뼄�뵳怨뺣쐭, 占쎈뼄�뵳怨뺣쐭 占쎈뼣 占쎄쉰占쎄숲 占쎈땾�몴占� �빊�뮆�젾
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // �룯占� 占쎄쉰占쎄숲 占쎈땾 �빊�뮆�젾
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 占쎈솁占쎌뵬沃섎챸苑ｆ에占� 占쎄퐜占쎈선占쎌궔 LBA 占쎈선占쎈굡占쎌쟿占쎈뮞占쎈퓠占쎄퐣 占쎄쉰占쎄숲 占쎈땾 筌띾슦寃� 占쎌뵭占쎌벉
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
    
    // 占쎈솁占쎌뵬沃섎챸苑� �뵳�딅뮞占쎈뱜�몴占� �룯�뜃由곤옙�넅占쎈릭占쎈연 LBA 占쎈선占쎈굡占쎌쟿占쎈뮞占쏙옙 占쎄쉰占쎄숲 占쎈땾 �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // 占쎄쉰占쎄숲 占쎈땾筌띾슦寃� 筌롫뗀�걟�뵳�됵옙占� 占쎈막占쎈뼣 獄쏆룇釉� 占쎌뵭疫뀐옙 占쎈땾占쎈뻬
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // 占쎈쑓占쎌뵠占쎄숲 甕곌쑵�쓠占쎌벥 占쎄땀占쎌뒠占쎌뱽 �빊�뮆�젾
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

                // 筌뤴뫀紐� 占쎈あ 占쎌쁽�뵳�됱쨮 占쎈ご占쎈뻻占쎈릭占쎌젻�⑨옙 16癰귣��뼄 占쎌삂占쏙옙 野껋럩�뒭 0占쎌뱽 �빊遺쏙옙占쎈퉸餓ο옙
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
 *  占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 占쎈솁占쎌뵬沃섎챸苑ｆ에占� 占쎄퐜占쎈선占쎌궔 LBA 占쎈선占쎈굡占쎌쟿占쎈뮞占쎈퓠占쎄퐣 占쎄쉰占쎄숲 占쎈땾 筌띾슦寃� 占쏙옙
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

    // 占쎈솁占쎌뵬沃섎챸苑� �뵳�딅뮞占쎈뱜�몴占� �룯�뜃由곤옙�넅占쎈릭占쎈연 LBA 占쎈선占쎈굡占쎌쟿占쎈뮞占쏙옙 占쎄쉰占쎄숲 占쎈땾 �빊遺욱뀱
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
    
    // 甕곌쑵�쓠�몴占� 占쎈막占쎈뼣 獄쏆룇釉� 占쎈쑓占쎌뵠占쎄숲�몴占� 筌�袁⑼옙.
    // 占쎈솭占쎄쉘占쏙옙 4 獄쏅뗄�뵠占쎈뱜占쎌벥 LBA 占쎈선占쎈굡占쎌쟿占쎈뮞占쏙옙 4 獄쏅뗄�뵠占쎈뱜占쎌벥 占쎈쾺疫꿸퀗占� 占쎈땾占쎈뻬占쎈쭆 占쎌뒒占쎈땾嚥∽옙 占쎄문占쎄쉐
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // 占쎈쾺疫뀐옙 占쎈땾占쎈뻬
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // 占쎈쑓占쎌뵠占쎄숲 甕곌쑵�쓠占쎌벥 占쎄땀占쎌뒠占쎌뱽 �빊�뮆�젾
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

            // 筌뤴뫀紐� 占쎈あ 占쎌쁽�뵳�됱쨮 占쎈ご占쎈뻻占쎈릭占쎌젻�⑨옙 16癰귣��뼄 占쎌삂占쏙옙 野껋럩�뒭 0占쎌뱽 �빊遺쏙옙占쎈퉸餓ο옙
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
 *  占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿�몴占� 占쎈염野껓옙
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
 *  占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�ο옙�뱽 占쎄문占쎄쉐(占쎈７筌랃옙)
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
 *  占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 占쎌젟癰귣�占쏙옙 占쎈ご占쎈뻻
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
 *  �뙴�뫂�뱜 占쎈탵占쎌젂占쎄숲�뵳�딅퓠 �뜮占� 占쎈솁占쎌뵬占쎌뱽 占쎄문占쎄쉐
 */
static void CreateFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_createdirectory(pcParameterBuffer);
}

static void CreateDirectory( const char* pcParameterBuffer )
{
	char name[256];
	PARAMETERLIST stList;

	// 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  �뙴�뫂�뱜 占쎈탵占쎌젂占쎄숲�뵳�딅퓠占쎄퐣 占쎈솁占쎌뵬占쎌뱽 占쎄텣占쎌젫
 */
static void DeleteFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  �뙴�뫂�뱜 占쎈탵占쎌젂占쎄숲�뵳�딆벥 占쎈솁占쎌뵬 筌뤴뫖以됵옙�뱽 占쎈ご占쎈뻻
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  占쎈솁占쎌뵬占쎌뱽 占쎄문占쎄쉐占쎈릭占쎈연 占쎄텕癰귣�諭뜻에占� 占쎌뿯占쎌젾占쎈쭆 占쎈쑓占쎌뵠占쎄숲�몴占� 占쏙옙
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 占쎈솁占쎌뵬沃섎챸苑� �뵳�딅뮞占쎈뱜�몴占� �룯�뜃由곤옙�넅占쎈릭占쎈연 占쎈솁占쎌뵬 占쎌뵠�뵳袁⑹뱽 �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 占쎈솁占쎌뵬 占쎄문占쎄쉐
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈퓦占쎄숲 占쎄텕揶쏉옙 占쎈염占쎈꺗占쎌몵嚥∽옙 3甕곤옙 占쎈땭占쎌쑎筌욑옙 占쎈르繹먮슣占� 占쎄땀占쎌뒠占쎌뱽 占쎈솁占쎌뵬占쎈퓠 占쏙옙
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // 占쎈퓦占쎄숲 占쎄텕占쎌뵠筌롳옙 占쎈염占쎈꺗 3甕곤옙 占쎈땭占쎌쑎鈺곕슢�뮉揶쏉옙 占쎌넇占쎌뵥占쎈릭占쎈연 �뙴�뫂遊썹몴占� �뜮醫롮죬 占쎄돌揶쏉옙
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // 占쎈퓦占쎄숲 占쎄텕揶쏉옙 占쎈툡占쎈빍占쎌뵬筌롳옙 占쎈퓦占쎄숲 占쎄텕 占쎌뿯占쎌젾 占쎌뒒占쎈땾�몴占� �룯�뜃由곤옙�넅
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
 *  占쎈솁占쎌뵬占쎌뱽 占쎈였占쎈선占쎄퐣 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎌뵭占쎌벉
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 占쎈솁占쎌뵬沃섎챸苑� �뵳�딅뮞占쎈뱜�몴占� �룯�뜃由곤옙�넅占쎈릭占쎈연 占쎈솁占쎌뵬 占쎌뵠�뵳袁⑹뱽 �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 占쎈솁占쎌뵬 占쎄문占쎄쉐
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈솁占쎌뵬占쎌벥 占쎄국繹먮슣占� �빊�뮆�젾占쎈릭占쎈뮉 野껉퍔�뱽 獄쏆꼶�궗
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // 筌띾슣鍮� 占쎈퓦占쎄숲 占쎄텕占쎌뵠筌롳옙 占쎈퓦占쎄숲 占쎄텕 占쎌뒒占쎈땾�몴占� 筌앹빓占쏙옙�뻻占쎄텕�⑨옙 20占쎌뵬占쎌뵥繹먮슣占� �빊�뮆�젾占쎈뻥占쎈뼄筌롳옙
        // 占쎈쐭 �빊�뮆�젾占쎈막筌욑옙 占쎈연�겫占썹몴占� �눧�눘堉깁걡占�
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
 *  占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�ο옙�벥 筌�癒��뻻 甕곌쑵�쓠占쎈퓠 占쎌뿳占쎈뮉 占쎈쑓占쎌뵠占쎄숲�몴占� 筌뤴뫀紐� 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 占쏙옙
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
 *  占쎈뻻�뵳�딅섰 占쎈７占쎈뱜嚥≪뮆占쏙옙苑� 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎈땾占쎈뻿占쎈릭占쎈연 占쎈솁占쎌뵬嚥∽옙 占쏙옙占쎌삢
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
    
    // 占쎈솁占쎌뵬沃섎챸苑� �뵳�딅뮞占쎈뱜�몴占� �룯�뜃由곤옙�넅占쎈릭占쎈연 占쎈솁占쎌뵬 占쎌뵠�뵳袁⑹뱽 �빊遺욱뀱
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
    
    // 占쎈뻻�뵳�딅섰 占쎈７占쎈뱜占쎌벥 FIFO�몴占� 筌뤴뫀紐� �뜮袁⑼옙
    ClearSerialFIFO();
    
    //==========================================================================
    // 占쎈쑓占쎌뵠占쎄숲 疫뀀챷�뵠揶쏉옙 占쎈땾占쎈뻿占쎈쭍 占쎈르繹먮슣占� 疫꿸퀡�뼄�뵳怨뺣뼄占쎈뮉 筌롫뗄�뻻筌욑옙�몴占� �빊�뮆�젾占쎈릭�⑨옙, 4 獄쏅뗄�뵠占쎈뱜�몴占� 占쎈땾占쎈뻿占쎈립 占쎈츟
    // Ack�몴占� 占쎌읈占쎈꽊
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // 占쎄텚占쏙옙 占쎈땾筌띾슦寃� 占쎈쑓占쎌뵠占쎄숲 占쎈땾占쎈뻿
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // 占쎈땾占쎈뻿占쎈쭆 占쎈쑓占쎌뵠占쎄숲揶쏉옙 占쎈씨占쎈뼄筌롳옙 占쎌삛占쎈뻻 占쏙옙疫뀐옙
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // 占쏙옙疫꿸퀬釉� 占쎈뻻揶쏄쑴�뵠 30�룯占� 占쎌뵠占쎄맒占쎌뵠占쎌뵬筌롳옙 Time Out占쎌몵嚥∽옙 餓λ쵐占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // 筌띾뜆占쏙쭕�맩�몵嚥∽옙 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎈땾占쎈뻿占쎈립 占쎈뻻揶쏄쑴�뱽 揶쏄퉮�뻿
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // 占쎌젟占쎄맒占쎌읅占쎌몵嚥∽옙 占쎈쑓占쎌뵠占쎄숲 疫뀀챷�뵠�몴占� 占쎈땾占쎈뻿占쎈뻥占쎌몵沃섓옙嚥∽옙, Ack�몴占� 占쎈꽊占쎈뻿
    SendSerialData( "A", 1 );

    //==========================================================================
    // 占쎈솁占쎌뵬占쎌뱽 占쎄문占쎄쉐占쎈릭�⑨옙 占쎈뻻�뵳�딅섰嚥≪뮆占쏙옙苑� 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎈땾占쎈뻿占쎈릭占쎈연 占쎈솁占쎌뵬占쎈퓠 占쏙옙占쎌삢
    //==========================================================================
    // 占쎈솁占쎌뵬 占쎄문占쎄쉐
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈쑓占쎌뵠占쎄숲 占쎈땾占쎈뻿
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // 甕곌쑵�쓠占쎈퓠 占쎈뼖占쎈툡占쎄퐣 占쎈쑓占쎌뵠占쎄숲�몴占� 占쏙옙
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // 占쎌뵠甕곕뜆肉� 占쎈쑓占쎌뵠占쎄숲揶쏉옙 占쎈땾占쎈뻿占쎈쭆 野껉퍔�뵠 占쎌뿳占쎈뼄筌롳옙 ACK 占쎌굢占쎈뮉 占쎈솁占쎌뵬 占쎈쾺疫뀐옙 占쎈땾占쎈뻬
        if( dwTempSize != 0 ) 
        {
            // 占쎈땾占쎈뻿占쎈릭占쎈뮉 筌잛럩占� 占쎈쑓占쎌뵠占쎄숲占쎌벥 筌띾뜆占쏙쭕�맦�돱筌욑옙 占쎈땾占쎈뻿占쎈뻥椰꾧퀡援� FIFO占쎌벥 占쎄쾿疫꿸퀣�뵥
            // 16 獄쏅뗄�뵠占쎈뱜筌띾뜄�뼄 占쎈립甕곕뜆逾� Ack�몴占� 占쎌읈占쎈꽊
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // 占쎈쾺疫뀐옙 餓λ쵐肉� �눧紐꾩젫揶쏉옙 占쎄문疫꿸퀡�늺 獄쏅뗀以� �넫�굝利�
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // 筌띾뜆占쏙쭕�맩�몵嚥∽옙 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎈땾占쎈뻿占쎈립 占쎈뻻揶쏄쑴�뱽 揶쏄퉮�뻿
            qwLastReceivedTickCount = GetTickCount();
        }
        // 占쎌뵠甕곕뜆肉� 占쎈땾占쎈뻿占쎈쭆 占쎈쑓占쎌뵠占쎄숲揶쏉옙 占쎈씨占쎈뼄筌롳옙 占쎌삛占쎈뻻 占쏙옙疫뀐옙
        else
        {
            Sleep( 0 );
            
            // 占쏙옙疫꿸퀬釉� 占쎈뻻揶쏄쑴�뵠 10�룯占� 占쎌뵠占쎄맒占쎌뵠占쎌뵬筌롳옙 Time Out占쎌몵嚥∽옙 餓λ쵐占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // 占쎌읈筌ｏ옙 占쎈쑓占쎌뵠占쎄숲占쎌벥 占쎄쾿疫꿸퀣占� 占쎈뼄占쎌젫嚥∽옙 占쎈땾占쎈뻿 獄쏆룇占� 占쎈쑓占쎌뵠占쎄숲占쎌벥 占쎄쾿疫꿸퀡占쏙옙 �뜮袁㏉꺍占쎈릭占쎈연 占쎄쉐�⑨옙 占쎈연�겫占썹몴占�
    // �빊�뮆�젾占쎈립 占쎈츟, 占쎈솁占쎌뵬占쎌뱽 占쎈뼍�⑨옙 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 筌�癒��뻻�몴占� 筌뤴뫀紐� �뜮袁⑼옙
    //==========================================================================
    // 占쎈땾占쎈뻿占쎈쭆 疫뀀챷�뵠�몴占� �뜮袁㏉꺍占쎈퉸占쎄퐣 �눧紐꾩젫揶쏉옙 獄쏆뮇源�占쎈뻥占쎈뮉筌욑옙�몴占� 占쎈ご占쎈뻻
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // 占쎈솁占쎌뵬占쎌뱽 占쎈뼍�⑨옙 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 筌�癒��뻻�몴占� 占쎄땀癰귣�源�
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP 占쎄퐬占쎌젟 占쎈�믭옙�뵠�뇡占� 占쎌젟癰귣�占쏙옙 �빊�뮆�젾
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ占쏙옙 I/O APIC占쎌벥 占쎌뵥占쎄숲占쎌쓦占쎈뱜 占쎌뿯占쎌젾 占쏙옙(INTIN)占쎌벥 �꽴占썸�④쑬占쏙옙 占쏙옙占쎌삢占쎈립 占쎈�믭옙�뵠�뇡遺우뱽 占쎈ご占쎈뻻
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC�몴占� �꽴占썹뵳�뗫릭占쎈뮉 占쎌쁽�뙴�슡�럡鈺곌퀣肉� 占쎌뿳占쎈뮉 �빊�뮆�젾 占쎈맙占쎈땾�몴占� 占쎌깈�빊占�
    PrintIRQToINTINMap();
}

/**
 *  �굜遺용선 癰귢쑬以� 占쎌뵥占쎄숲占쎌쓦占쎈뱜�몴占� 筌ｌ꼶�봺占쎈립 占쎌뒒占쎈땾�몴占� �빊�뮆�젾
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
    
    // MP 占쎄퐬占쎌젟 占쎈�믭옙�뵠�뇡遺용퓠 占쏙옙占쎌삢占쎈쭆 �굜遺용선占쎌벥 揶쏆뮇�땾�몴占� 占쎌뵭占쎌벉
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column �빊�뮆�젾
    //==========================================================================
    // 占쎈늄嚥≪뮇苑�占쎄퐣占쎌벥 占쎈땾筌띾슦寃� Column占쎌뱽 �빊�뮆�젾
    // 占쎈립 餓κ쑴肉� �굜遺용선 4揶쏆뮇逾� �빊�뮆�젾占쎈릭�⑨옙 占쎈립 Column占쎈뼣 15燁삳챷�뱽 占쎈막占쎈뼣占쎈맙
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
        
        // �빊�뮆�젾占쎈릭�⑨옙 占쎄텚占쏙옙 �⑤벀而숋옙�뱽 筌뤴뫀紐� 占쎈뮞占쎈읂占쎌뵠占쎈뮞嚥∽옙 筌�袁⑼옙
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row占쏙옙 占쎌뵥占쎄숲占쎌쓦占쎈뱜 筌ｌ꼶�봺 占쎌뒒占쎈땾 �빊�뮆�젾
    //==========================================================================
    // �룯占� 占쎌뵥占쎄숲占쎌쓦占쎈뱜 占쎌뒒占쎈땾占쏙옙 �굜遺용선 癰귨옙 占쎌뵥占쎄숲占쎌쓦占쎈뱜 筌ｌ꼶�봺 占쎌뒒占쎈땾�몴占� �빊�뮆�젾
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row�몴占� �빊�뮆�젾, 占쎈립 餓κ쑴肉� �굜遺용선 4揶쏆뮇逾� �빊�뮆�젾占쎈릭�⑨옙 占쎈립 Column占쎈뼣 15燁삳챷�뱽 占쎈막占쎈뼣
            if( j == 0 )
            {
                // 20 占쎌뵬占쎌뵥筌띾뜄�뼄 占쎌넅筌롳옙 占쎌젟筌욑옙
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
            // �빊�뮆�젾占쎈릭�⑨옙 占쎄텚占쏙옙 占쎌겫占쎈열占쎌뱽 筌뤴뫀紐� 占쎈뮞占쎈읂占쎌뵠占쎈뮞嚥∽옙 筌�袁⑼옙
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
 *  占쎄묶占쎈뮞占쎄쾿占쎌벥 占쎈늄嚥≪뮇苑�占쎄퐣 燁살뮉�넅占쎈즲�몴占� 癰귨옙野껓옙
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // 占쎄묶占쎈뮞占쎄쾿 ID 占쎈툡占쎈굡 �빊遺욱뀱
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 占쎈늄嚥≪뮇苑�占쎄퐣 燁살뮉�넅占쎈즲(Affinity) �빊遺욱뀱
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
 *  VBE 筌뤴뫀諭� 占쎌젟癰귨옙 �뇡遺얠쨯占쎌뱽 �빊�뮆�젾
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE 筌뤴뫀諭� 占쎌젟癰귨옙 �뇡遺얠쨯占쎌뱽 獄쏆꼹�넎
    pstModeInfo = GetVBEModeInfoBlock();
    
    // 占쎈퉸占쎄맒占쎈즲占쏙옙 占쎄퉳 占쎌젟癰귣�占쏙옙 占쎌맄雅뚯눖以� �빊�뮆�젾
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
 *  占쎈뻻占쎈뮞占쎈�� �굜�뮇�뱽 占쎈�믭옙�뮞占쎈뱜占쎈릭占쎈뮉 占쎌�占쏙옙 占쎌쟿甕곤옙 占쎄묶占쎈뮞占쎄쾿�몴占� 占쎄문占쎄쉐
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // 占쎈짗占쎌읅 占쎈막占쎈뼣 占쎌겫占쎈열占쎈퓠 4Kbyte 筌롫뗀�걟�뵳�됵옙占� 占쎈막占쎈뼣 獄쏆룇釉� 占쎌�占쏙옙 占쎌쟿甕곤옙 占쎄묶占쎈뮞占쎄쾿�몴占� 占쎄문占쎄쉐占쎈막 餓ο옙�뜮袁⑨옙占� 占쎈맙
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // 占쎌�占쏙옙 占쎌쟿甕곤옙 占쎄묶占쎈뮞占쎄쾿嚥∽옙 占쎄텢占쎌뒠占쎈막 SystemCallTestTask() 占쎈맙占쎈땾占쎌벥 �굜遺얜굡�몴占� 占쎈막占쎈뼣 獄쏆룇占� 筌롫뗀�걟�뵳�딅퓠 癰귣벊沅�
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // 占쎌�占쏙옙 占쎌쟿甕곤옙 占쎄묶占쎈뮞占쎄쾿嚥∽옙 占쎄문占쎄쉐
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
 // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
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
 *  占쎌벓占쎌뒠占쎈늄嚥≪뮄�젃占쎌삪占쎌뱽 占쎈뼄占쎈뻬
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // 占쎈솁占쎌뵬沃섎챸苑ｇ몴占� �빊遺욱뀱
    InitializeParameter( &stList, pcParameterBuffer );
    // 占쎌굨占쎈뻼占쎈퓠 筌띿쉸占� 占쎈륫占쎌몵筌롳옙 占쎈즲占쏙옙筌띾Ŋ�뱽 �빊�뮆�젾占쎈릭�⑨옙 �넫�굝利�
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // 占쎈あ 甕곕뜆�럮 占쎌뵥占쎌쁽 �눧紐꾩쁽占쎈였占쏙옙 占쎌긿占쎈�∽옙�몵嚥∽옙 筌ｌ꼶�봺
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // 占쎄묶占쎈뮞占쎄쾿 占쎄문占쎄쉐
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  占쎈솭占쎄텕筌욑옙占쎈퓠 占쎈굶占쎈선占쎌뿳占쎈뮉 占쎈쑓占쎌뵠占쎄숲�몴占� 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 癰귣벊沅�
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

    // �겫占쏙옙�뱜 嚥≪뮆�쐭揶쏉옙 嚥≪뮆逾ワ옙留� 0x7C05 占쎈선占쎈굡占쎌쟿占쎈뮞占쎈퓠占쎄퐣 癰귣똾�깈 筌뤴뫀諭� �뚣끇瑗멩�⑨옙 IA-32e 筌뤴뫀諭� �뚣끇瑗몌옙�뱽
    // 占쎈�占쎈립 占쎄쉰占쎄숲 占쎈땾�몴占� 占쎌뵭占쎌벉
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // 占쎈탵占쎈뮞占쎄쾿 占쎌뵠沃섎챷占쏙옙�뮉 0x10000 占쎈선占쎈굡占쎌쟿占쎈뮞占쎈퓠 嚥≪뮆逾ワ옙由븃첋占썸에占� 占쎌뵠�몴占� 疫꿸퀣占쏙옙�몵嚥∽옙
    // �뚣끇瑗� 占쎄쉰占쎄숲 占쎈땾筌띾슦寃� 占쎈샵占쎈선筌욑옙 �⑤끃肉� 占쎈솭占쎄텕筌욑옙 占쎈엘占쎈쐭揶쏉옙 占쎌뿳占쎌벉
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // 占쎈뻻域밸챶瑗ワ㎗�꼶占쏙옙 占쎌넇占쎌뵥
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // 占쎈솭占쎄텕筌욑옙 占쎄땀占쎌벥 筌뤴뫀諭� 占쎈솁占쎌뵬占쎌뱽 筌≪뼚釉섓옙苑� 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿占쎈퓠 癰귣벊沅�
    //--------------------------------------------------------------------------
    // 占쎈솭占쎄텕筌욑옙 占쎈쑓占쎌뵠占쎄숲揶쏉옙 占쎈뻻占쎌삂占쎈릭占쎈뮉 占쎈선占쎈굡占쎌쟿占쎈뮞
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // 占쎈솭占쎄텕筌욑옙 占쎈엘占쎈쐭占쎌벥 筌ｏ옙 甕곕뜆�럮 占쎈솁占쎌뵬 占쎈쑓占쎌뵠占쎄숲
    pstItem = pstHeader->vstItem;

    // 占쎈솭占쎄텕筌욑옙占쎈퓠 占쎈７占쎈맙占쎈쭆 筌뤴뫀諭� 占쎈솁占쎌뵬占쎌뱽 筌≪뼚釉섓옙苑� 癰귣벊沅�
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // 占쎈솭占쎄텕筌욑옙占쎈퓠 占쎈７占쎈맙占쎈쭆 占쎈솁占쎌뵬 占쎌뵠�뵳袁⑹몵嚥∽옙 占쎈솁占쎌뵬占쎌뱽 占쎄문占쎄쉐
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // 占쎈솭占쎄텕筌욑옙 占쎈쑓占쎌뵠占쎄숲 �겫占썽겫袁⑸퓠 占쎈７占쎈맙占쎈쭆 占쎈솁占쎌뵬 占쎄땀占쎌뒠占쎌뱽 占쎈릭占쎈굡 占쎈탵占쎈뮞占쎄쾿嚥∽옙 癰귣벊沅�
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // 占쎈솁占쎌뵬占쎌뱽 占쎈뼍�⑨옙 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 筌�癒��뻻�몴占� 占쎄땀癰귣�源�
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // 占쎈솁占쎌뵬占쎌뱽 占쎈뼍占쎌벉
        fl_fclose( fp );

        // 占쎈뼄占쎌벉 占쎈솁占쎌뵬占쎌뵠 占쏙옙占쎌삢占쎈쭆 占쎌맄燁살꼶以� 占쎌뵠占쎈짗
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // 占쎈솁占쎌뵬 占쎈뻻占쎈뮞占쎈�� 筌�癒��뻻�몴占� 占쎄땀癰귣�源�
   // FlushFileSystemCache();
}

