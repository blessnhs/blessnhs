/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �뜝�럡���뜝�럡�꽠�뜝�럡�꺓 占쎈쐻占쎈윥椰꾩�λ쐻占쎈윥占쎈군 �뜝�럡�뒋占쎈쐻占쎈짗占쎌굲�뜝�럩二⑨옙�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥�댆戮⑸쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙
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

#include "NETPROTOCOL/socket.h"
#include "NETPROTOCOL/if.h"
#include "MODULE/ftp.h"
// �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥�꼧�뜝�럥��占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎌젳占쎈쐻占쎈윪甕곤옙
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
        { "createfile", "Create File, ex)createfile a.txt", CreateFile },
        { "del", "Delete File, ex)deletefile a.txt", DeleteFile },
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
		{"svr","svr",SVR},
};

//==============================================================================
//  占쎈쐻占쎈윥�젆袁��쐻占쎈윪占쎌졆 占쎈쐻占쎈윥椰꾩�λ쐻占쎈윪獄�占� �뜝�럥夷ⓨ뜝�럥遊쏙옙�렰占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 �뜝�럡��占쎄껀占쎈폇�뤃占�
//==============================================================================
/**
 *  占쎈쐻占쎈윥椰꾩�λ쐻占쎈윪甕곤옙 癲ル슢�닪占쎈씔�뜝�럥�뜲 �뜝�럥���뜝�럥爾쒙옙嫄▼뜝占�
 */

void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // �뜝�럡���뜝�럡�꽠�뜝�럡�꺓占쎈쐻占쎈윪獄�占� �뜝�럡�뒋占쎈쐻占쎈쑟�얜냲�삕占쎈엮�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윪占쎄껑�뜝�럥���뜝�럩�뮚�뜝�럥�쐾占쎈뙑�⑤슦占썩뼹�쐻占쎈짗占쎌굲 占쎈쎗占쎈즵�몭諭꾩삕占쎄섬
    pstConsoleManager = GetConsoleManager();
    
    // 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈뮟占쎈엮占쎈뭷占쎈쐻占쎈윥獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // �뜝�럡���뜝�럡�꽠�뜝�럡�꺓 占쎈쐻占쎈윥椰꾬옙 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕 占쎈쐻占쎈윥占쎄뭇占쎈쐻占쎈윪占쎄데占쎌쐺獄쏅ı�듋占쎈쐻�뜝占� TRUE占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥�몴�떦萸먪솒占쏙옙�뮝占쎈쐻�뜝占� 占쎈쎗占쎈즵�몭�씛�삕亦낉옙
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // �뜝�럡���뜝�럡�꽠�뜝�럡�꺓 占쎈쐻占쎈윥椰꾬옙 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕 占쎈쐻占쎈윥占쎄뭇占쎈쐻占쎈윪占쎄데占쎌쐺獄쏅ı�듋占쎈쐻�뜝占� 占쎈쐻占쎈윞占쎈쭫占쎈쐻占쎈윪占쎌젳占쎈쐻占쎈윥壤쏉옙 占쎈눇�뙼�뿫�쑋�뜝�럥裕� �뜝�럥���뜝�럥爾쒙옙嫄∽옙�쑟筌뤿떣�쐻�뜝占� �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
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
                // 占쎈쐻占쎈윪野껉막�쐻占쎈윪占쎄땍 �뜝�럥�뒗占쎄콬占쎈빝�뜝占� 占쎈쐻占쎈윪筌띻쑈�뇲占쎄땀�몭�쑚�쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄숱占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥�뵳占� �뜝�럥�떛嶺뚮ㅎ�뫒占쎄껑 占쎈쐻占쎈윥筌띻낑�쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욎�λ쐻占쎈윥�뵳占� 占쎈쐻占쎈윥�젆袁��쐻占쎈윪甕곤옙 �뜝�뜦維�甕곕맕泥롳옙�뼠占쎌굲�뜝�럥援� �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲
                // �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 癲ル슢�뵯占쎌맆占쎈쐻占쎈짗壤쏅틶�쐻�뜝占� �뜝�럥�떛嶺뚮ㅎ�뫒占쎄껑 占쎈쐻占쎈윞占쎈��占쎈쐻占쎈윪占쎌졆
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
                // �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윥占쎈군 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 癲ル슢�뀖�뤃�빍占쎄퀡逾껓옙�굲�뜝�럥援� 占쎈쐻占쎈윥�젆袁��쐻占쎈윥筌묕옙
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈뮟占쎈엮占쎈뭷占쎈쐻占쎈윥獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議� 占쎈쎗占쎈즸占쎌굲 �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈탶�⑤슣維볟뜝�럩諭� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗�
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥占쎈뭷占쎈쐻占쎈윥獄�占� 占쎈쐻占쎈윞占쎈��, CAPS Lock, NUM Lock, Scroll Lock占쎈쐻占쎈짗占쎌굲 �뜝�럥�떛�뜝�럥�깓�뜝�럥六�
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB占쎈쐻占쎈짗占쎌굲 �뜝�뜦維�甕곕맕泥롳옙�뼠占쎌굲�뜝�럥�걬占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪占쎌벁占쎈쐻占쎈윪占쎄섬
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // 占쎈탶�⑤슣維볟뜝�럩諭븝옙琉놅옙猷딉옙�굲 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈윥占쎈떋占쎈쐻占쎈윪�굢�뀘�쐻占쎈윪獄�占� 占쎈쐻占쎈윥�몴�떦異�占쎌뿪占쎌굲 占쎈쨬占쎈즸占쎌굲占쎈쐻占쎈윥獒뺧옙
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
 *  �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윥占쎈군 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�댙�삕筌뤿떣�쐻�뜝占� �뜝�럥�몡�넭怨λ즸�댆�씢�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈뤅占쎈쐻占쎈윥�젆占� �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�댙�삕筌뤿떣�쐻�뜝占� 癲ル슪�맋�몭�씛�삕�걡酉대쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥筌띿눨�쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �뜝�뜦維�甕곕맕泥롳옙�뼠占쎌굲�뜝�럥�걬占쎌뒙占쎈뙔占쎌굲 �뜝�럥夷ⓨ뜝�럥留わ옙�뿥�쐻占쎈윥壤쏉옙 �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�댙�삕筌뤿떣�쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃占� 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥�꼧�뜝�럥��占쎄껀占쎌뒭獄�占� 占쎈눇�뙼蹂��굲占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윥占쎈뤅占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥筌욎�λ쐻占쎈윪�앗껊쐻占쎈윥�뵳占� 占쎈쐻占쎈윪�얠쥜�삕�얜뀡伊덌옙維쀨린占� �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�넂琉놅옙猷딉옙�굲 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺣맟異�占쎌돸占쎌굲 占쎈쐻占쎈윪占쎄섈占쎈쐻占쎈윪�억옙
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪甕곤옙 占쎈섀占쏙옙筌�琉꾩삕�얠±�쐻占쎈짗占쎌굲 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윪占쎄괜占쎈쐻占쎈윪占쎌벁占쎈쐻占쎈윪�굢占� 占쎈쐻占쎈윪�앗낅뇲占쎄땀�몭諛깅눀�뜝�뜴�쐻占쎈윥獒뺣맟異�占쎌돸占쎌굲 占쎈눇�뙼蹂��굲占쎈쐻占쎈윞占쎈��
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뭿�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 癲ル슓�룱�젆�떣�삕獄�占� 占쎈쐻占쎈윥占쎈묄 占쎈쐻占쎈윥占쎈뎁占쎈쐻占쎈윥�젆袁ㅼ땡嚥〓끃�굲 占쎈쐻占쎈윥占쎈군占쎈쐻占쎈윪占쎈첓 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� 占쎈쐻占쎈윪占쎄껑�뜝�럥���뜝�럩�뮚�뜝�럥�쐾占쎈뙑�⑤슦占썩뼹�쐻占쎈짗占쎌굲 �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗�
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �뜝�뜦維�甕곕맕泥롳옙�뼠占쎌굲�뜝�럥�걬占쎌뒙占쎈뙔占쎌굲 �뜝�럥夷ⓨ뜝�럥留わ옙�뿥�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影�瑜곸굲�뜝�럥爰� 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛�뜝�뜦維낉옙�굲 占쎈섀占쏙옙筌�琉꾩삕�얠쥜�삕筌뤿떣�쐻�뜝占� 占쎈쎗占쎈즵�몭諭꾩삕占쎄섬
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // 占쎈쐻占쎈윥占쎈쭬 占쎈쐻占쎈윪�얠±�쐻占쎈윞筌랃옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��굞苡몌옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈뎁占쎈쐻占쎈윪筌뤿뱺異�嚥〓끃�굲 占쎈쐻占쎈윞占쎈즼占쎈쨬占쎈즸占쎌굲
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윪甕곤옙 占쎈섀占쏙옙筌�琉꾩삕�얠쥉異�占쎌뵯占쎈뮡�뇦猿볦삕 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욎�λ쐻占쎈윥�뵳占쏙┼�슢�닪占쎌깓占쎈빝�뜝占� �뜝�뜦維�甕곕맕泥롳옙�뼠占쎌굲�뜝�럥援� 占쎈눇�뙼蹂��굲占쎈쐻占쎈윞占쎈룿
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� 占쎌녃域뱄퐢苡답벧�굞占쏙옙占쎌굲占쎈눀�뜝�룞�삕占쎈쳟占쎌굲 占쎈섀占쏙옙筌�琉꾩삕�얠쥜�삕筌뤿떣�쐻�뜝占� 占쎈쎗占쎈즵�몭諭꾩삕占쎄섬
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影�瑜곸굲�뜝�럥爰� 占쎈쐻占쎈윪筌띻쑈�뇲占쎄땔占쎌굲 占쎈쐻占쎈윥占쎈턀占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윥獄�占�
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�댙�삕筌뤿떣�쐻�뜝占� 癲ル슪�맋�몭�씛�삕�걡酉대쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 �뜝�럡��占쎄껀占쎈폇�뤃占�
//==============================================================================
/**
 *  占쎈쐻占쎈윥椰꾬옙 占쎈쐻占쎈윥筌앷엥�쐻占쎈짗占쎌굲癲ル슢�뵯�씙�뜝�럥援� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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

    // 占쎈쨬占쎈즸占쎌굲占쎈쐻占쎈윪占쎄텑 占쎈섀占쎈Ŋ�굲 �뜝�럥�뒗占쎄콪�뜝�럥�젌占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪甕곤옙 占쎈섀占쏙옙筌�琉꾩삕�얠쥜�삕筌뤿떣�쐻�뜝占� �뜝�뜦紐울옙維믢벧�굢�삕
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // 占쎈쐻占쎈윥筌앷엥�쐻占쎈짗占쎌굲癲ル슢�뿪占쎌굲 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // 癲ル슢�뀈泳�戮⑤뭄占쎈㎜占쎌굲�뜝�럥�꼧 癲ル슢�뵯占쎌꼻�삕獄�占� 占쎈눇�뙼�뿫�쑋�뜝�럥裕� 占쎈쐻占쎈윞占쎈즼占쎈쐻占쎈윥占쎈뼁占쎈쐻占쎈윞占쎈쭓 占쎌녃域뱄퐢�깓占쎄덩占쎄랜萸꾬옙�쐻�뜝占�
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
 *  占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓�뜝�럥援� 癲ル슣�돸占쎌굲占쎈쐻占쎈짗占쎌굲
 */
static void Cls( const char* pcParameterBuffer )
{

    // 癲ル슢�뿪占쎌굲 占쎈쐻占쎈윪筌랃옙濚욌꼬釉먮쳮占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎄틣占쎈탶�⑤슣維끻뜝�럥猷� 占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛占쎈쐻占쎈윥�뵳占썰벧猿뗪섶占쎌굲占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓�뜝�럥援� 癲ル슣�돸占쎌굲占쎈쐻占쎈윪占쎈�� 占쎈쐻占쎈윪占쎌맗, 占쎈쐻占쎈윪�앗껊쐻占쎈윪�억옙 1占쎌뒙占쎈뙔占쎌굲 �뜝�럥�뒗占쎄콬占쎈빝�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욑옙
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // 癲ル슢�뿪占쎌굲 占쎈쐻占쎈윪筌랃옙濚욌꼬釉먮쳮占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎄틣占쎈탶�⑤슣維끻뜝�럥猷� 占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛占쎈쐻占쎈윥�뵳占썰벧猿뗪섶占쎌굲占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓�뜝�럥援� 癲ル슣�돸占쎌굲占쎈쐻占쎈윪占쎈�� 占쎈쐻占쎈윪占쎌맗, 占쎈쐻占쎈윪�앗껊쐻占쎈윪�억옙 1占쎌뒙占쎈뙔占쎌굲 �뜝�럥�뒗占쎄콬占쎈빝�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욑옙
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

int ftpmain()
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
	// 癲ル슢�뿪占쎌굲 占쎈쐻占쎈윪筌랃옙濚욌꼬釉먮쳮占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎄틣占쎈탶�⑤슣維끻뜝�럥猷� 占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛占쎈쐻占쎈윥�뵳占썰벧猿뗪섶占쎌굲占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓�뜝�럥援� 癲ル슣�돸占쎌굲占쎈쐻占쎈윪占쎈�� 占쎈쐻占쎈윪占쎌맗, 占쎈쐻占쎈윪�앗껊쐻占쎈윪�억옙 1占쎌뒙占쎈뙔占쎌굲 �뜝�럥�뒗占쎄콬占쎈빝�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욑옙
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
 *  �뜝�럥竊륅옙�쐻�뜝占� 癲ル슢�닪占쏙옙�뜝�럡肄ュ뜝�럥�렓占쎈쐻�뜝占� 占쎈쐻占쎈윞�떋�슌堉②퐛紐낉옙�뼹�쐻占쎈짗占쎌굲 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎄땍占쎈쐻占쎈윥筌묒궍�쐻占쎈윪占쎄탾(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 癲ル슪�삕占쎌넂�뜝�룞�삕筌묒궍�쐻占쎈윥占쎈군 占쎈쐻占쎈윥�뤃�쑚�쐻占쎈윥占쎄퐨占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪獄�占� 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윪亦끤룸춴�솒�곸굲
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
    // 占쎈쐻占쎈윞占쎈�욑옙�녃域뱄퐦�삕�뛾占썲뜝占� �뜝�럥�뒖泳�洹⑥삕獄��뭿�뒙占쎈섞占쎄콪�뜝�럩紐졾뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥�땻寃쇰쐻占쎈윥占쎈뤅 PC�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎄땍占쎈쐻占쎈윥筌묒궍�쐻占쎈윪占쎄탾
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윥�댆�λ쐻占쎈윥筌앸�먯삕筌뤿떣�쐻�뜝占� 癲ル쉵��猷긷뜝�럩�젳
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10�뜝�럥竊륅옙�쐻�뜝占� 占쎈쐻占쎈윥筌욎�λ쐻占쎈윥占쎈떛 占쎌녃域밟뫁�굲占쎈쐻占쎈윪占쎄섀占쎈쐻占쎈윥�뵳占� 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪�굢占� 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞占쎈빣占쎈쐻占쎈윥占쎈뭷 占쎈뇲占쎄텭占쎌깓�뜝�럥裕㏆옙�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윪占쎈츛占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윥�댆�λ쐻占쎈윥筌앸�먯삕筌뤿떣�쐻�뜝占� 占쎈쨬占쎈즲占쎈쳮�뜝�럩�젞占쎈쐻占쎈윪占쎌뱾占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 癲ル쉵��猷긷뜝�럩�젳
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪�얠쥜�삕占쎈꼥占쎈쐻�뜝占� 占쎌녃域뱄퐢苡닷뜝�럥�맱
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC �뜝�럥�뒖泳�洹⑥삕獄��뭿�뒙占쎈섞占쎄콪�뜝�럩紐좑옙�쐻占쎈윥占쎈군 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑占쎈쐻占쎈윥壤쏉옙 占쎈쐻占쎈윪�앗껊쐻占쎈윪占쎄껑 占쎈쎗占쎈즸占쎌굲 占쎈쐻占쎈윥筌묒궏琉놅옙猷딉옙�굲 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC �뜝�럥�뒖泳�洹⑥삕獄��뭿�뒙占쎈섞占쎄콪�뜝�럩紐좑옙�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥筌묒궏琉놅옙猷딉옙�굲 占쎈쎗占쎈즸占쎌굲 占쎈쐻占쎈윪�앗껊쐻占쎈윪占쎄껑�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윪占쎈�∽옙�쐻占쎈윞占쎈쭡 占쎈쐻占쎈윥占쎈뻘占쎈쐻占쎈윪筌띻쒀�삕筌뤿떣�쐻�뜝占� 占쎌녃域밟뫁�굲占쎈눇�뙼蹂��굲
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윪占쎈�∽옙�쐻占쎈윞占쎈쭡 占쎈쐻占쎈윥占쎈뻘占쎈쐻占쎈윪筌띻쒀�삕筌뤿떣�쐻�뜝占� 占쎌녃域밟뫁�굲占쎈눇�뙼蹂��굲
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
 *  占쎈쐻占쎈윪野껉막�쐻占쎈윪占쎄땍 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰占쎈쐻占쎈윥壤쏉옙 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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
    
    // �뜝�럡��占쎄껀占쎌뒠占쎄퐨 占쎈쐻占쎈윥占쎈묄癲ル슢�뵯占쎈뮡�뇦猿볦삕 �뜝�럥���뜝�럥爾쒙옙嫄∽옙�쑟筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윥筌앹뇴異�嚥▲꺂�깓占쎈빝�뜝占� 占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥獒뺤쉻�삕占쎈꺋占쎌굲濚욌꼬釉먮쳥�뜝�럩紐좑옙�쐻占쎈윥占쎈군 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈쭬占쎈쐻占쎈윥筌랃옙
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // �뜝�럡��占쎄껀占쎌뒠占쎄퐨占쎈쨬占쎈즸占쎌굲 2占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윪�얠±�쐻占쎈윞筌띾�ｋ쐻占쎈윪�얠쥉異�嚥〓끃�굲 占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥獒뺤쉻�삕占쎈꺋占쎌굲濚욌꼬釉먮쳥�뜝�럩紐� 占쎌녃域뱄옙占쎈쳥繞벿우삕 占쎈쨬占쎈즵獒뺣돍�삕占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    if( iProcessorCount > 1 )
    {
        // 占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥獒뺤쉻�삕占쎈꺋占쎌굲濚욌꼬釉먮쳥�뜝�럩紐� 占쎌녃域뱄옙占쎈쳥繞벿우삕 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쨬占쎈즵獒뺣돍�삕占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈짗占쎌굲 �뜝�뜦維�甕곤옙占쎈슣�땬占쎌굲�뜝�럥援� 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎌뱻占쎈쐻占쎈윪�얠±�쐻占쎈윥獒뺤쉫�쎗占쎈즴占쏙옙繞벿우삕 癲ル슪�삕�넭怨ｋ쳹占쎌굲
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
        // TCB�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥夷ⓨ뜝�럥肉ワ옙�뤅占쎈쐻占쎈윞占쎈쭓 TCB占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛 濚욌꼬�궡�꺍�뜝�럥�꼧癲ル슢�뺧옙�굲 ID�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슆琉놅옙猷딉옙�굲 6占쎈쨬占쎈즸占쎌굲 �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥�몴�떦異�占쎌뵯占쎌맄�뜝�럥�렡, �뜝�뜦紐울옙維믣뜝�럡�떗 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋占� 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묒궍�쐻占쎈윥筌띾맟異�占쎌돸占쎌굲 占쎈쐻占쎈윥占쎈염�뜝�럡猿�占쎈쐻占쎈쑟筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윪占쎄섈占쎈쐻占쎈윪�억옙
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
 *  占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 占쎈쐻占쎈윥獄��엺�쐻占쎈윪占쎌젳 ID癲ル슢�뿪占쎌굲 �뜝�럥苑욃뜝�럡�꼤嶺뚯빖�닰占쎌굲占쎈눀�뜝�뜴�쐻占쎈윥獒뺧옙 占쎈눇�뙼�뿫�쑋�뜝�럥裕�
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥痢⑼옙�쐻占쎈윥獄��뭿�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윪占쎌졆占쎈쐻占쎈윪占쎈눞
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
    // �뜝�럡���뜝�럡�꽠�뜝�럡�꺓 占쎈쐻占쎈윥椰꾩���삕占쎈쳟占쎌굲 占쎈쐻占쎈윪�뜝�뜴�쐻占쎈윪筌륅옙 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윪占쎌졆占쎈쐻占쎈윪占쎈눞占쎈쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋占� �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥痢⑼옙�쐻占쎈윥獄��뭿�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윞占쎈��占쎈쐻占쎈윪占쎌졆 癲ル슢�뀈泳�戮⑤뭄占쎈㎜占쎌굲占쎄덩占쎈㎜占쎌굲占쎈빝�뜝占� 占쎈쐻占쎈윪占쎌졆占쎈쐻占쎈윪占쎈눞
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
 *  占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛�뜝�럥�걫占쎈꽞嚥∽옙獄�占� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // 占쎈쨬占쎈즸占쎌굲 �뜝�럡��占쎄껀占쎌뒠占쎄퐨 占쎌녃域뱄옙占쎈쳥繞벿우삕 �뜝�럡猿�占쎈쐻占쎈짗占쎌굲占쎈눀�뜝�룞�삕筌뤿떣�쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈짗占쎌굲 �뜝�뜦維�甕곤옙占쎈슣�땬占쎌굲�뜝�럥援� 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎌뱻占쎈쐻占쎈윪�얠±�쐻占쎈윥獒뺤쉫�쎗占쎈즴占쏙옙繞벿우삕 癲ル슪�삕�넭怨ｋ쳹占쎌굲
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// 占쎈쐻占쎈윞占쎈�딉옙�쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쎗占쎈즵獒뺣뎾�떐�뜝�뜴�쐻占쎈윥筌묒궍�쐻占쎈윞占쎈�욑옙堉⑨옙癒��굲 占쎈쐻占쎈윪筌띻쐼�쐻占쎈윥�뵳占� 占쎌녃域밟뫁�굲占쎈쐻占쎈윥占쎈묄
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  占쎈쐻占쎈윪�굢�깷�쐻占쎈윪甕겄듬쐻占쎈윪甕곤옙 占쎈쐻占쎈윞占쎈�딉옙�쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쎗占쎈즵�몭諭꾩삕占쎄섬
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  癲ル슪�맋鴉딆닂�삕占쎄껑�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪甕겄듬쐻占쎈윪占쎈첓占쎈쐻占쎈윞占쏙옙�뜝�럥�렓�뜝�럥�쐠占쎈쿊 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥�뤃占�
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // 占쎈쐻占쎈윪占쎄�占쎈쐻占쎈윥筌묕옙 占쎈쐻占쎈짗占쎌굲占쎈섀饔낅챸占싼뉖눀�뜝占�
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
 *  占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥�뤃�댙�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 癲ル슢�뵞占쎄괍�뜝�럥肄��뜝�럥�렓占쎈쐻占쎈짗占쎌굲�뜝�럥痢� 占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓占쎈쐩域밟뫁�굲�뜝�럩諭� 占쎌녃域뱄퐢�깓占쎄덩占쎄랩�뻿�뜝�럥�땻�뜝�럥痢� 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윥獒뺧옙
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

    // 占쎈쐻占쎈윞占쎈�욑옙琉놅옙猷딉옙�굲 占쎈쐻占쎈윪�굢占쏙옙�쐻占쎈윪占쎌죷占쎈쐻占쎈윥占쎈돌癲ル슢�뺧옙�굲 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윥獒뺧옙 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
    GetCh();
}

/**
 *  癲ル슢�뵞占쎄괍�뜝�럥肄��뜝�럥�렓占쎈쐻占쎈짗占쎌굲�뜝�럥痢� 占쎈쐻占쎈윪占쎄섀癲ル슢�닪占쎌깓�뜝�럥援� 占쎌녃域뱄퐢�깓占쎄덩占쎄랜萸꾬옙�쐻�뜝占�
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슆琉놅옙猷딉옙�굲 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕 占쎈쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥�몴�떦萸먪솒占쏙옙�뮝占쎈쐻�뜝占� 占쎈쐻占쎈짗占쎌굲占쎈섀占쎈Ŋ�굲
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
 *  占쎈쐻占쎈윥筌욎�λ쐻占쎈윪占쎌뱾 癲ル슢�닪占쏙옙�뜝�럡肄ュ뜝�럥�렓占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
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
 *  占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // 癲ル슢�뀈泳�占썲뜝�럩紐� 占쎈탶�⑤베�맇�뜝�럡�돭 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // 占쎈쐻占쎈윥筌묒궪�삕�얜냲�삕占쎈늅占쎄숲 占쎈탶�⑤베�맇�뜝�럡�돭 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // 占쎈쐻占쎈윥占쎈퓮占쎈쐻占쎈윥�뤃占�, 占쎈쐻占쎈윥�젆袁ъ삕�얜뀛占썩뫀踰좑옙留�, 占쎈쐻占쎈윥�젆袁ъ삕�얜뀛占썩뫀踰좑옙留� 占쎈쐻占쎈윥�젆占� 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // �뜝�럥竊륅옙�쐻�뜝占� 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��냱肉됵옙�쐻�뜝占� 占쎈쐻占쎈윞占쎈쭆占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윪亦낉옙 LBA 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄 癲ル슢�뵯占쎈뮡�뇦猿볦삕 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
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
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗⑨옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 LBA 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈짗占쎌굲 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄 �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄癲ル슢�뵯占쎈뮡�뇦猿볦삕 癲ル슢�닪占쏙옙�뜝�럡肄ュ뜝�럥�렓�뜝�럥留�占쎌굲占쎈쐻�뜝占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占� 占쎈쎗占쎈즵�뙴�돁�눀�뜝占� 占쎈쐻占쎈윪�억옙占쎈섀占쎈Ŋ�굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윪甕곤옙 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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

                // 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥占쎄콟 占쎈쐻占쎈윪占쎄껑�뜝�럥�렓�뜝�럥留⒴ㅇ占� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묒궍�쐻占쎈윥�뵳占쏙옙�쐻占쎈윪占쎌죳�뜝�뜦維낉옙�굲 16占쎌녃域뱄퐦�삕�뜝�럥�렡 占쎈쐻占쎈윪占쎄탾占쎈쐻占쎈짗占쎌굲 占쎈눇�뙼�뿫�쑋�뜝�럥裕� 0占쎈쐻占쎈윪獄�占� �뜝�럥�돯占쎄껀占쎈짗占쎌굲占쎈쐻占쎈윥占쎈뤅濚욌꼬�슦�굲
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
 *  占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��냱肉됵옙�쐻�뜝占� 占쎈쐻占쎈윞占쎈쭆占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윪亦낉옙 LBA 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄 癲ル슢�뵯占쎈뮡�뇦猿볦삕 占쎈쐻占쎈짗占쎌굲
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

    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗⑨옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 LBA 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈짗占쎌굲 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄 �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
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
    
    // 占쎈탶�⑤슣維볟뜝�럩諭뷴뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占� 占쎈쎗占쎈즵�뙴�돁�눀�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 癲ル슪�삕�넭怨ｋ쳹占쎌굲.
    // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈룈占쎈쐻占쎈짗占쎌굲 4 占쎈쎗占쎈즴占쎈씔�뜝�럥�꼧占쎈쐻占쎈윥獄��뭿�쐻占쎈윪甕곤옙 LBA 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈짗占쎌굲 4 占쎈쎗占쎈즴占쎈씔�뜝�럥�꼧占쎈쐻占쎈윥獄��뭿�쐻占쎈윪甕곤옙 占쎈쐻占쎈윥�떋釉�堉②퐛紐낉옙�λ쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌뫱껊쐻占쎈윥壤쏉옙 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // 占쎈쐻占쎈윥�떋釉�堉⑨옙癒��굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윪甕곤옙 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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

            // 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥占쎄콟 占쎈쐻占쎈윪占쎄껑�뜝�럥�렓�뜝�럥留⒴ㅇ占� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묒궍�쐻占쎈윥�뵳占쏙옙�쐻占쎈윪占쎌죳�뜝�뜦維낉옙�굲 16占쎌녃域뱄퐦�삕�뜝�럥�렡 占쎈쐻占쎈윪占쎄탾占쎈쐻占쎈짗占쎌굲 占쎈눇�뙼�뿫�쑋�뜝�럥裕� 0占쎈쐻占쎈윪獄�占� �뜝�럥�돯占쎄껀占쎈짗占쎌굲占쎈쐻占쎈윥占쎈뤅濚욌꼬�슦�굲
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
 *  占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윥占쎈옘占쎈눇�뙼蹂��굲
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
 *  占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�떥�슦�굲�뜝�럥援� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰(占쎈쐻占쎈윥塋딆�㏃땡占쎌뿪占쎌굲)
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
 *  占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
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
 *  �뜝�럥���뜝�럥爾쒎뜝�럥肄� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윪占쎌읆占쎈쐻占쎈윞占쎈뙃�뜝�럥�렓�뜝�럥遊억옙諭� �뜝�럥�몡占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
 */
static void CreateFile( const char* pcParameterBuffer )
{
	char name[256];
	char buffer[1024];
	PARAMETERLIST stList;

	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );
	GetNextParameter( &stList, buffer );


	void *fd = fl_fopen(name,"w");
	if(fd == 0)
	{
		Printf("%s open fail\n");
		return ;
	}

	fl_fputs(buffer,fd);

	fl_fclose(fd);
}

static void CreateDirectory( const char* pcParameterBuffer )
{
	char name[256];
	PARAMETERLIST stList;

	// 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  �뜝�럥���뜝�럥爾쒎뜝�럥肄� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윪占쎌읆占쎈쐻占쎈윞占쎈뙃�뜝�럥�렓�뜝�럥遊억옙諭븝옙�쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윞占쎈��占쎈쐻占쎈윪占쎌졆
 */
static void DeleteFile( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  �뜝�럥���뜝�럥爾쒎뜝�럥肄� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윪占쎌읆占쎈쐻占쎈윞占쎈뙃�뜝�럥�렓�뜝�럥遊욤린占� 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 癲ル슢�뀈泳�戮⑤뭄占쎈㎜占쎌굲�뜝�럥援� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윞占쎈�욑옙�녃域뱄퐦�삕�뛾占쏙옙�몴占쎈퓠占쎈쐻�뜝占� 占쎈쐻占쎈윪�굢占쏙옙�쐻占쎈윪占쎌죷占쎈쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈짗占쎌굲
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗⑨옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윪�얠쥜�삕�얜뀡伊덌옙維쀨キ占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�욑옙琉놅옙猷딉옙�굲 占쎈쐻占쎈윥占쎈옘占쎈쐻占쎈윥�댆�λ쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 3占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윥占쎈퉮占쎈쐻占쎈윪占쎈첓癲ル슣�돸占쎌굲 占쎈쐻占쎈윥�몴�떦萸먪솒占쏙옙�뮝占쎈쐻�뜝占� 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪獄�占� 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윥占쎈군 占쎈쐻占쎈짗占쎌굲
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�욑옙�쐻占쎈윪�얠쥉異�嚥〓끃�굲 占쎈쐻占쎈윥占쎈옘占쎈쐻占쎈윥�댆占� 3占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윥占쎈퉮占쎈쐻占쎈윪占쎈첓占쎈뙑�⑤벡�뮛�뜝�럥痢롳옙琉놅옙猷딉옙�굲 占쎈쐻占쎈윪占쎄섈占쎈쐻占쎈윪�앓듬쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 �뜝�럥���뜝�럥爾쒙옙嫄∽옙�쑟筌뤿떣�쐻�뜝占� �뜝�럥�몡占쎈꽞嚥∽옙雅뚳옙 占쎈쐻占쎈윞占쎈즼占쎈쨬占쎈즸占쎌굲
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�욑옙琉놅옙猷딉옙�굲 占쎈쐻占쎈윥占쎈떋占쎈쐻占쎈윥�뜮�씢�쐻占쎈윪�앗낆땡嚥〓끃�굲 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�� 占쎈쐻占쎈윪�굢占쏙옙�쐻占쎈윪占쎌죷 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗�
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
 *  占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗⑨옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윪�얠쥜�삕�얜뀡伊덌옙維쀨キ占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪甕곤옙 占쎈쐻占쎈윞�뤃占썸퓴諛매�占쎈뮝占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈눇�뙼�맪�쐭�뜝�럥援� 占쎈쎗占쎈즵�몭�씛�삕亦낉옙
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // 癲ル슢�뵯占쎈뮝占쎈쑏�뜝占� 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�욑옙�쐻占쎈윪�얠쥉異�嚥〓끃�굲 占쎈쐻占쎈윥占쎈�占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윞占쎈�� 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 癲ル슣鍮섌뜮蹂잙쐻占쎈짗占쎌굲�뜝�럥六ｏ옙�쐻占쎈윞占쎈�욃뜝�뜦維낉옙�굲 20占쎈쐻占쎈윪�앗껊쐻占쎈윪�앓룸춴�솒占쏙옙�뮝占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥筌뫮듬쐻占쎈윥�젆袁ㅼ땡嚥〓끃�굲
        // 占쎈쐻占쎈윥占쎈쭬 �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥筌띾맟異�占쎌돸占쎌굲 占쎈쐻占쎈윥占쎈염�뜝�럡猿�占쎈쐻占쎈쑟筌뤿떣�쐻�뜝占� �뜝�럥�떛�뜝�럥�땾占쎌젂繹먭낫愿뚳옙�쐻�뜝占�
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
 *  占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�떥�슦�굲�뜝�럥爰� 癲ル슪�삕占쎌넂�뜝�룞�삕筌묕옙 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윥占쎈군 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎈쐻占쎈짗占쎌굲
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
 *  占쎈쐻占쎈윥筌묒궪�삕�얜냲�삕占쎈늅占쎄숲 占쎈쐻占쎈윥塋딆�λ쐻占쎈윥獄��뭿�뒙占쎈룱獒뺣끀�쐻占쎈짗占쎌굲占쎈빝�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껋뒙占쎈뙔占쎌굲 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑
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
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝�뜝占� �뜝�럥�렓�뜝�럥遊얕짆�쉩�쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� �뜝�럥竊뤷뜝�럥�맃占쎈뎨�ⓦ끉�굲�뜝�럥瑗⑨옙�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윪�얠쥜�삕�얜뀡伊덌옙維쀨キ占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
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
    
    // 占쎈쐻占쎈윥筌묒궪�삕�얜냲�삕占쎈늅占쎄숲 占쎈쐻占쎈윥塋딆�λ쐻占쎈윥獄��뭿�쐻占쎈윪甕곤옙 FIFO�뜝�럥�걫占쎈쐻�뜝占� 癲ル슢�뀈泳�占썹춯瑜낆삕 �뜝�럥�몡�넭怨ｋ쳹占쎌굲
    ClearSerialFIFO();
    
    //==========================================================================
    // 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈섀占쏙옙筌�琉꾩삕�얠×琉놅옙猷딉옙�굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥�몴�떦萸먪솒占쏙옙�뮝占쎈쐻�뜝占� 占쎈섀饔낅챸占썩댙�삕�젆袁ъ삕�얜뀛占썩뫀踰졾젆袁��쐻占쎈윥獒뺧옙 癲ル슢�닪占쎈씔�뜝�럥六ｏ┼�슣�돸占쎌굲�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲, 4 占쎈쎗占쎈즴占쎈씔�뜝�럥�꼧占쎈쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占� 占쎈쐻占쎈윥筌ο옙
    // Ack�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎌벁占쎈쐻占쎈윥�땻占�
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈짗占쎌굲 占쎈쐻占쎈윥占쎈묄癲ル슢�뵯占쎈뮡�뇦猿볦삕 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥占쎈뎁占쎈쐻占쎈윥�젆袁ㅼ땡嚥〓끃�굲 占쎈쐻占쎈윪占쎄�占쎈쐻占쎈윥筌묕옙 占쎈쐻占쎈짗占쎌굲占쎈섀占쎈Ŋ�굲
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // 占쎈쐻占쎈짗占쎌굲占쎈섀饔낅챸占싼뉖눀�뜝占� 占쎈쐻占쎈윥筌묒궏琉놅옙猷꾬옙維믣뜝�럥�꼧 30�뜝�럥竊륅옙�쐻�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윞筌띾�ｋ쐻占쎈윪�얠±�쐻占쎈윪�앗낆땡嚥〓끃�굲 Time Out占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 濚욌꼬�궡�꺍占쎈쐻�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // 癲ル슢�뵯占쎌맆占쎈쐻占쎈짗壤쏅벨�삕筌띯뫜�삕筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占� 占쎈쐻占쎈윥筌묒궏琉놅옙猷꾬옙維믣뜝�럥援� 占쎈쨬占쎈즲占쎈룶�뜝�럥堉�
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // 占쎈쐻占쎈윪占쎌젳占쎈쐻占쎈윞筌띾�ｋ쐻占쎈윪占쎌뱾占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈섀占쏙옙筌�琉꾩삕�얠쥜�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥筌뫮듬쐻占쎈윪筌뤿뱷苡э옙瑗삼옙�굲占쎌뒙占쎈뙔占쎌굲, Ack�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥�땻�벩�쐻占쎈윥筌묕옙
    SendSerialData( "A", 1 );

    //==========================================================================
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰占쎈쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윥筌묒궪�삕�얜냲�삕占쎈늅占쎄숲占쎌뒙占쎈룱獒뺣끀�쐻占쎈짗占쎌굲占쎈빝�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윥占쎈군 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑
    //==========================================================================
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // 占쎈탶�⑤슣維볟뜝�럩諭븝옙�쐻占쎈윥占쎈군 占쎈쐻占쎈윥�젆戮⑸쐻占쎈윥占쎈떋占쎈쐻占쎈윞占쎈쭓 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈짗占쎌굲
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // 占쎈쐻占쎈윪�얠쥉逾볠�⑤베�맆占쎄덩�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥壤쏉옙 占쎈눇�뙼�맪�쐭�뜝�럥�꼧 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥�젆袁ㅼ땡嚥〓끃�굲 ACK 占쎈쐻占쎈윪�뤃�꽒�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥�떋釉�堉⑨옙癒��굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙
        if( dwTempSize != 0 ) 
        {
            // 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 癲ル슣�삎占쎌쑋占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쐻占쎈윪甕곤옙 癲ル슢�뵯占쎌맆占쎈쐻占쎈짗壤쏅벨�삕筌랃옙�뜝�럥琉�癲ル슣�돸占쎌굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥筌뫮뜰�熬곥룊占썩넂琉껃뜝占� FIFO占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윞�떋�슌堉②퐛紐낉옙節륁삕�억옙
            // 16 占쎈쎗占쎈즴占쎈씔�뜝�럥�꼧占쎈쐻占쎈윥獄��뮁異�占쎌뵯占쎌맄�뜝�럥�렡 占쎈쐻占쎈윥�뵳�뜾逾볠�⑤베�맆占쎌뼲�삕 Ack�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎌벁占쎈쐻占쎈윥�땻占�
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // 占쎈쐻占쎈윥�떋釉�堉⑨옙癒��굲 濚욌꼬�궡�꺍占쎄덩�뜝占� �뜝�럥�떛嶺뚮ㅎ�뫒占쎌졆占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윞�눧硫곕섀饔낅챸占썩댙�삕占쎈뱠 占쎈쎗占쎈즴占쏙옙繞벿우삕 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // 癲ル슢�뵯占쎌맆占쎈쐻占쎈짗壤쏅벨�삕筌띯뫜�삕筌뤿뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥�뵳占� 占쎈쐻占쎈윥筌묒궏琉놅옙猷꾬옙維믣뜝�럥援� 占쎈쨬占쎈즲占쎈룶�뜝�럥堉�
            qwLastReceivedTickCount = GetTickCount();
        }
        // 占쎈쐻占쎈윪�얠쥉逾볠�⑤베�맆占쎄덩�뜝占� 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥占쎈뎁占쎈쐻占쎈윥�젆袁ㅼ땡嚥〓끃�굲 占쎈쐻占쎈윪占쎄�占쎈쐻占쎈윥筌묕옙 占쎈쐻占쎈짗占쎌굲占쎈섀占쎈Ŋ�굲
        else
        {
            Sleep( 0 );
            
            // 占쎈쐻占쎈짗占쎌굲占쎈섀饔낅챸占싼뉖눀�뜝占� 占쎈쐻占쎈윥筌묒궏琉놅옙猷꾬옙維믣뜝�럥�꼧 10�뜝�럥竊륅옙�쐻�뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윞筌띾�ｋ쐻占쎈윪�얠±�쐻占쎈윪�앗낆땡嚥〓끃�굲 Time Out占쎈쐻占쎈윪筌뤿뱶�뒙占쎈뙔占쎌굲 濚욌꼬�궡�꺍占쎈쐻�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // 占쎈쐻占쎈윪占쎌벁癲ル슪�맔占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윞�떋�슌堉②퐛紐낉옙節낅쐻�뜝占� 占쎈쐻占쎈윥�젆袁��쐻占쎈윪占쎌졆占쎌뒙占쎈뙔占쎌굲 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묕옙 占쎈쎗占쎈즵�뙴�뎽�쐻�뜝占� 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윞�떋�슌堉②퐛紐낉옙�뼹�쐻占쎈짗占쎌굲 �뜝�럥�몡�넭怨λ즸�댆�씢�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥占쎈염 占쎈쐻占쎈윞占쎈렰�뜝�뜦維낉옙�굲 占쎈쐻占쎈윥占쎈염�뜝�럡猿�占쎈쐻占쎈쑟筌뤿떣�쐻�뜝占�
    // �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占� 占쎈쐻占쎈윥筌ο옙, 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윥�젆�엪�삕占쎈쳟占쎌굲 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 癲ル슪�삕占쎌넂�뜝�룞�삕筌묒궪�삕筌뤿떣�쐻�뜝占� 癲ル슢�뀈泳�占썹춯瑜낆삕 �뜝�럥�몡�넭怨ｋ쳹占쎌굲
    //==========================================================================
    // 占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈윥筌묒슃�쐻占쎈윥壤쏉옙 占쎈섀占쏙옙筌�琉꾩삕�얠쥜�삕筌뤿떣�쐻�뜝占� �뜝�럥�몡�넭怨λ즸�댆�씢�쐻占쎈윥占쎈뤅占쎈쐻占쎈윞占쎈쭓 �뜝�럥�떛嶺뚮ㅎ�뫒占쎌졆占쎈쨬占쎈즸占쎌굲 占쎈쎗占쎈즵獒뺣뎾�떐�뜝�뜴�쐻占쎈윥筌뫮듬쐻占쎈윥獒뺣맟異�占쎌돸占쎌굲�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윥�젆�엪�삕占쎈쳟占쎌굲 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 癲ル슪�삕占쎌넂�뜝�룞�삕筌묒궪�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞占쏙옙占쎌녃域뱄퐦�삕濚밸㉡�삕
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP 占쎈쐻占쎈윞占쎈쭫占쎈쐻占쎈윪占쎌젳 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥�꼧�뜝�럥��占쎈쐻�뜝占� 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ占쎈쐻占쎈짗占쎌굲 I/O APIC占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윪�앓듬쐻占쎈윞占쎈뙃占쎈쐻占쎈윪占쎈�占쎈쐻占쎈윥獄�占� 占쎈쐻占쎈윪�굢占쏙옙�쐻占쎈윪占쎌죷 占쎈쐻占쎈짗占쎌굲(INTIN)占쎈쐻占쎈윪甕곤옙 �뜝�럡�뒋占쎈쐻占쎈쑞�뜝�뜦紐울옙維딉옙�쐻占쎈짗占쎌굲 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑占쎈쐻占쎈윥�뵳占� 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥�꼧�뜝�럥��占쎄껀占쎌뒭獄�占� 占쎈쐻占쎈윥占쎄괴占쎈쐻占쎈윥筌묕옙
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC�뜝�럥�걫占쎈쐻�뜝占� �뜝�럡�뒋占쎈쐻占쎈쑟�얜냲�삕占쎈엮�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윪占쎄껑�뜝�럥���뜝�럩�뮚�뜝�럥�쐾占쎈뙑�⑤슦占쏙퐟援℡뜝占� 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 �뜝�럥�돯�뜝�럥痢듿뜝�럩議� 占쎈쐻占쎈윥筌띿눨�쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪繹먮뜉�삕�뜮�벩�쐻�뜝占�
    PrintIRQToINTINMap();
}

/**
 *  �뜝�럡��占쎄껀占쎌뒠占쎄퐨 占쎌녃域뱄옙占쎈쳥繞벿우삕 占쎈쐻占쎈윪�앓듬쐻占쎈윞占쎈뙃占쎈쐻占쎈윪占쎈�占쎈쐻占쎈윥獄��뮋�삕筌뤿떣�쐻�뜝占� 癲ル슪�맋�몭�씛�삕�걡酉대쐻占쎈윥�뵳占� 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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
    
    // MP 占쎈쐻占쎈윞占쎈쭫占쎈쐻占쎈윪占쎌젳 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥�꼧�뜝�럥��占쎄껀占쎌뒠占쎈군 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑占쎈쐻占쎈윥壤쏉옙 �뜝�럡��占쎄껀占쎌뒠占쎄퐨占쎈쐻占쎈윪甕곤옙 占쎈쨬占쎈즵獒뺣돍�삕占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    //==========================================================================
    // 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓占쎈쐻占쎈윪甕곤옙 占쎈쐻占쎈윥占쎈묄癲ル슢�뵯占쎈뮡�뇦猿볦삕 Column占쎈쐻占쎈윪獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    // 占쎈쐻占쎈윥�뵳占� 濚욌꼬釉먮쳮占쎄덩�뜝占� �뜝�럡��占쎄껀占쎌뒠占쎄퐨 4占쎈쨬占쎈즵獒뺣돁占쎌뼲�삕 �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윥�뵳占� Column占쎈쐻占쎈윥�젆占� 15占쎈뇲占쎄텭筌�琉꾩삕獄�占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆節낅쐻占쎈윥筌랃옙
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
        
        // �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈짗占쎌굲 �뜝�뜦維�甕곤옙占쎈슣�땬占쎌굲�뜝�럥援� 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎌뱻占쎈쐻占쎈윪�얠±�쐻占쎈윥獒뺤쉩�뒙占쎈뙔占쎌굲 癲ル슪�삕�넭怨ｋ쳹占쎌굲
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윪�앓듬쐻占쎈윞占쎈뙃占쎈쐻占쎈윪占쎈�占쎈쐻占쎈윥獄�占� 癲ル슪�맋�몭�씛�삕�걡占� 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    //==========================================================================
    // �뜝�럥竊륅옙�쐻�뜝占� 占쎈쐻占쎈윪�앓듬쐻占쎈윞占쎈뙃占쎈쐻占쎈윪占쎈�占쎈쐻占쎈윥獄�占� 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄占쎈쐻占쎈짗占쎌굲 �뜝�럡��占쎄껀占쎌뒠占쎄퐨 占쎌녃域밟뫁�굲 占쎈쐻占쎈윪�앓듬쐻占쎈윞占쎈뙃占쎈쐻占쎈윪占쎈�占쎈쐻占쎈윥獄�占� 癲ル슪�맋�몭�씛�삕�걡占� 占쎈쐻占쎈윪占쎈츆占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row�뜝�럥�걫占쎈쐻�뜝占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�, 占쎈쐻占쎈윥�뵳占� 濚욌꼬釉먮쳮占쎄덩�뜝占� �뜝�럡��占쎄껀占쎌뒠占쎄퐨 4占쎈쨬占쎈즵獒뺣돁占쎌뼲�삕 �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윥�뵳占� Column占쎈쐻占쎈윥�젆占� 15占쎈뇲占쎄텭筌�琉꾩삕獄�占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占�
            if( j == 0 )
            {
                // 20 占쎈쐻占쎈윪�앗껊쐻占쎈윪�앓룹땡占쎌뵯占쎌맄�뜝�럥�렡 占쎈쐻占쎈윪占쎄섀癲ル슢�뺧옙�굲 占쎈쐻占쎈윪占쎌젳癲ル슣�돸占쎌굲
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
            // �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 占쎈쐻占쎈윞占쎈�ｏ옙�쐻占쎈짗占쎌굲 占쎈쐻占쎈윪野껁깷�쐻占쎈윥占쎈였占쎈쐻占쎈윪獄�占� 癲ル슢�뀈泳�占썹춯瑜낆삕 占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎌뱻占쎈쐻占쎈윪�얠±�쐻占쎈윥獒뺤쉩�뒙占쎈뙔占쎌굲 癲ル슪�삕�넭怨ｋ쳹占쎌굲
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
 *  占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윪甕곤옙 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓 占쎈뇲占쎄땀獒뺣맮�삕占쎄섀占쎈쐻占쎈윥筌앸�먯삕筌뤿떣�쐻�뜝占� 占쎌녃域밟뫁�굲占쎈눇�뙼蹂��굲
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋占� ID 占쎈쐻占쎈윥占쎈떋占쎈쐻占쎈윥�뤃占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺣돀�빝�뜝�뜴�쐻占쎈윞占쎈쭓 占쎈뇲占쎄땀獒뺣맮�삕占쎄섀占쎈쐻占쎈윥筌앾옙(Affinity) �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
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
 *  VBE 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윪占쎌젳占쎌녃域밟뫁�굲 �뜝�럥��占쎄껀占쎈폏鸚뤄옙占쎈쐻占쎈윪獄�占� �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윪占쎌젳占쎌녃域밟뫁�굲 �뜝�럥��占쎄껀占쎈폏鸚뤄옙占쎈쐻占쎈윪獄�占� 占쎈쎗占쎈즵�몭諭꾩삕占쎄섬
    pstModeInfo = GetVBEModeInfoBlock();
    
    // 占쎈쐻占쎈윥占쎈뤅占쎈쐻占쎈윞筌띾�ｋ쐻占쎈윥筌앷엥�쐻占쎈짗占쎌굲 占쎈쐻占쎈윞占쎈룿 占쎈쐻占쎈윪占쎌젳占쎌녃域뱄퐦�삕占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윪筌띻쑥�뜏占쎌뒩占쎈땻繞벿우삕 �뜝�럥�돯�뜝�럥痢듿뜝�럩議�
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
 *  占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 �뜝�럡���뜝�럥痢뗥뜝�럥援� 占쎈쐻占쎈윥�뜝�럥占쏙옙占쎌굲�뜝�럥痢⑼옙�쐻占쎈윥獄��뭿�쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윪�뜝�뜴�쐻占쎈짗占쎌굲 占쎈쐻占쎈윪占쎌쓨占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // 占쎈쐻占쎈윥筌욎�λ쐻占쎈윪占쎌뱾 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占� 占쎈쐻占쎈윪野껁깷�쐻占쎈윥占쎈였占쎈쐻占쎈윥占쎈군 4Kbyte 癲ル슢�닪占쏙옙�뜝�럡肄ュ뜝�럥�렓�뜝�럥留�占쎌굲占쎈쐻�뜝占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占� 占쎈쎗占쎈즵�뙴�돁�눀�뜝占� 占쎈쐻占쎈윪�뜝�뜴�쐻占쎈짗占쎌굲 占쎈쐻占쎈윪占쎌쓨占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슱�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰占쎈쐻占쎈윥筌랃옙 濚욌꼬�슦�굲�뜝�럥�몡�넭怨ｋ쳟占쎌굲占쎈쐻�뜝占� 占쎈쐻占쎈윥筌랃옙
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // 占쎈쐻占쎈윪�뜝�뜴�쐻占쎈짗占쎌굲 占쎈쐻占쎈윪占쎌쓨占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞占쎈�э옙�쐻占쎈윪占쎈츛占쎈쐻占쎈윥筌랃옙 SystemCallTestTask() 占쎈쐻占쎈윥筌띿눨�쐻占쎈윥占쎈묄占쎈쐻占쎈윪甕곤옙 �뜝�럡��占쎄껀占쎈폇�뤃�댙�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윥筌띾맕�쐻占쎈윥�젆占� 占쎈쎗占쎈즵�뙴�뎽�쐻�뜝占� 癲ル슢�닪占쏙옙�뜝�럡肄ュ뜝�럥�렓�뜝�럥遊억옙諭� 占쎌녃域뱄퐢苡답벧�굢�삕
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // 占쎈쐻占쎈윪�뜝�뜴�쐻占쎈짗占쎌굲 占쎈쐻占쎈윪占쎌쓨占쎈탶�ⓦ끉�굲 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
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

	 Printf("size %d %d\n",fd->filelength,fd->bytenum);

	 int i;
	 for(i=0;i<fd->filelength;i++)
		 Printf("%c",fl_fgetc(fd));

	 fl_fclose(fd);

}

static void SVR(const char* pcParameterBuffer )
{

	netif_t *netif = netif_findbyname ("eth0");

	sockaddr_in serverSock;

	int ncsock;
	// Create socket
	if ((ncsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			Printf("Cant create socket\n");
		}

	// Fill structure sockaddr_in
	// 1) Family of protocols
	serverSock.sin_family = AF_INET;
	// 2) Number of server port
	serverSock.sin_port = htons(20001);
	// 3) Setup ip address of server, where we want to connect
	serverSock.sin_addr = net_proto_ip_convert(netif->ip);

	net_proto_ip_print(serverSock.sin_addr);

	bind(ncsock,&serverSock,sizeof(sockaddr_in));

	char buffer[1024];

	while(1)
	{
			sockaddr_in newsd;
			socklen_t len;
			int sfd = accept(ncsock,&newsd,&len);

			memset(buffer,0,1000);
			recv(sfd,buffer,1000,0);

			Printf("%s\n",buffer);
	}

	return;
}

static void Ping(const char* pcParameterBuffer )
{
	int sock;
	char sendBuffer[2048];
	char recvBuffer[2048];

	sock = connectServer2("54.180.123.125",1982 );

	int idx = 0;
	while(1)
	{
		send(sock,"0123456789",10,0);

		int len = recv(sock, recvBuffer, 2048,0);

		if(len <= 0)
			continue;

		recvBuffer[len] = 0;

		Printf ("%d_%s\n",idx,recvBuffer);

		idx++;

	}

	sclose(sock);
	return;

	PARAMETERLIST stList;
	char ip[30];
 // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
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
 *  占쎈쐻占쎈윪甕곕퀫�쐻占쎈윪占쎈츛占쎈쐻占쎈윥占쎈뭷占쎌뒙占쎈룱獒뺢쒀�삕占쎌읇占쎈쐻占쎈윪占쎄텞占쎈쐻占쎈윪獄�占� 占쎈쐻占쎈윥�젆袁��쐻占쎈윥筌묕옙
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗꾩쒀占쎄섬筌�硫깅빝影��돃�걫占쎈쐻�뜝占� �뜝�럥�돯占쎄껀占쎌뒮占쏙옙
    InitializeParameter( &stList, pcParameterBuffer );
    // 占쎈쐻占쎈윪�뤃�벂�쐻占쎈윥筌묒뇯�쐻占쎈윥占쎈군 癲ル슢�뵰占쎈뤅占쎈쐻�뜝占� 占쎈쐻占쎈윥�몴�깷�쐻占쎈윪筌뤿뱺異�嚥〓끃�굲 占쎈쐻占쎈윥筌앷엥�쐻占쎈짗占쎌굲癲ル슢�뵯�씙�뜝�럥援� �뜝�럥�돯�뜝�럥痢듿뜝�럩議묕옙�쐻占쎈윥�뵳占썲뜝�뜦維낉옙�굲 �뜝�럥苑욃뜝�럡�꼤嶺뚯빢�삕
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // 占쎈쐻占쎈윥占쎄콟 占쎈탶�⑤베�맆�뜝�럥�쑐 占쎈쐻占쎈윪�앓듬쐻占쎈윪占쎄껑 �뜝�럥�떛嶺뚮ㅎ�뫒占쎄껑占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윪疫뀁슃�쐻占쎈윥�뜝�뜦�뙔占쎌굲�뜝�럥�걬占쎌뒙占쎈뙔占쎌굲 癲ル슪�맋�몭�씛�삕�걡占�
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // 占쎈쐻占쎈윞�눧�쑚�쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲占쎈쐻占쎈윥占쎈군 占쎈쐻占쎈윥�뤃�쑚�쐻占쎈윥占쎄퐨占쎈쐻占쎈윪�굢�뀘�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎌녃域뱄퐢苡답벧�굢�삕
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

    // �뜝�럡猿�占쎈쐻占쎈짗占쎌굲�뜝�럥肄� 占쎌뒙占쎈룱獒뺣냵�삕占쎈쭬占쎈쨬占쎈즸占쎌굲 占쎌뒙占쎈룱獒뺣끋占쎌뻼爰덌옙�굲嶺뚮엪�삕 0x7C05 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎈군占쎈쐻占쎈윞占쎈쭓 占쎌녃域뱄퐢�깙�뜝�럡�돭 癲ル슢�뀈泳�占썹뛾占썲뜝占� �뜝�럥�뒗占쎄콪占쎈き筌롡뫜�삕占쎈쳟占쎌굲 IA-32e 癲ル슢�뀈泳�占썹뛾占썲뜝占� �뜝�럥�뒗占쎄콪占쎈き筌뤿슣�굲�뜝�럥援�
    // 占쎈쐻占쎈윥�뜝�뜴�쐻占쎈윥�뵳占� 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄�뜝�럥�걫占쎈쐻�뜝占� 占쎈쐻占쎈윪�억옙占쎈쐻占쎈윪甕곤옙
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋占� 占쎈쐻占쎈윪�얠×苡э옙瑗띰㎖猷잙쐻占쎈짗占쎌굲�뜝�럥痢� 0x10000 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윥占쎈군 占쎌뒙占쎈룱獒뺣끋占쎌뻼爰덌옙�굲占쎈뎨�뇡猿뗭쓸占쎈쐻占쎈쑞占쎈퓠占쎈쐻�뜝占� 占쎈쐻占쎈윪�얠쥜�삕筌뤿떣�쐻�뜝占� 占쎈섀饔낅챸占쏙퐛�쐻占쎈짗占쎌굲�뜝�럥�걬占쎌뒙占쎈뙔占쎌굲
    // �뜝�럥�뒗占쎄콪占쎈き�뜝占� 占쎈쐻占쎈윞占쎈룺占쎈쐻占쎈윞占쎈뙃 占쎈쐻占쎈윥占쎈묄癲ル슢�뵯占쎈뮡�뇦猿볦삕 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥占쎄퐨癲ル슣�돸占쎌굲 �뜝�뜦維�占쎄콡占쎄덩�뜝占� 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲 占쎈쐻占쎈윥占쎈퓮占쎈쐻占쎈윥占쎈쭬占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윪�굢�뀘�쐻占쎈윪甕곤옙
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // 占쎈쐻占쎈윥筌묒궍�쐺獄쏅챷援∽옙紐�占쎄틛占쎈읁�뜝�럡�꽟占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윪占쎄섈占쎈쐻占쎈윪�억옙
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪甕곤옙 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 癲ル슓�룱�젆�떘�눀占쎄섶占쎌굲占쎈빝�뜝占� 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�쐻占쎈윥占쎈군 占쎌녃域뱄퐢苡답벧�굢�삕
    //--------------------------------------------------------------------------
    // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃占쎈쨬占쎈즸占쎌굲 占쎈쐻占쎈윥筌묒궍�쐻占쎈윪占쎄탾占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쎄퐨占쎈쐻占쎈윥�뤃�뼹�쐻占쎈윪占쎌쓨占쎈쐻占쎈윥獒뺧옙
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲 占쎈쐻占쎈윥占쎈퓮占쎈쐻占쎈윥占쎈쭬占쎈쐻占쎈윪甕곤옙 癲ル슪�맔占쎌굲 占쎈탶�⑤베�맆�뜝�럥�쑐 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃
    pstItem = pstHeader->vstItem;

    // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲占쎈쐻占쎈윥占쎈군 占쎈쐻占쎈윥塋딆�λ쐻占쎈윥筌띿눨�쐻占쎈윥壤쏉옙 癲ル슢�뀈泳�占썹뛾占썲뜝占� 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 癲ル슓�룱�젆�떘�눀占쎄섶占쎌굲占쎈빝�뜝占� 占쎌녃域뱄퐢苡답벧�굢�삕
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲占쎈쐻占쎈윥占쎈군 占쎈쐻占쎈윥塋딆�λ쐻占쎈윥筌띿눨�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윪�얠쥜�삕�얜뀡伊덌옙維쀯쭗�뱶�뒙占쎈뙔占쎌굲 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윞�눧硫⑤쐻占쎈윞占쎈렰
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // 占쎈쐻占쎈윥占쎈꽡占쎈쐻占쎈윞占쎈�욑┼�슣�돸占쎌굲 占쎈쐻占쎈윥占쎈ぅ占쎈쐻占쎈윪�얠±�쐻占쎈윞占쎈뙃 �뜝�럡猿�占쎈쐻占쎈쓡野껁깿伊덌옙維뽳옙諭� 占쎈쐻占쎈윥塋딆�λ쐻占쎈윥筌띿눨�쐻占쎈윥壤쏉옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윪占쎈츛占쎈쐻占쎈윪獄�占� 占쎈쐻占쎈윥�뵳占쏙옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎄틣占쎈쐻占쎈윥獒뺤쉩�쐻占쎈윞�떋�슃�뒙占쎈뙔占쎌굲 占쎌녃域뱄퐢苡답벧�굢�삕
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윥�젆�엪�삕占쎈쳟占쎌굲 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 癲ル슪�삕占쎌넂�뜝�룞�삕筌묒궪�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞占쏙옙占쎌녃域뱄퐦�삕濚밸㉡�삕
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪獄�占� 占쎈쐻占쎈윥�젆�씢�쐻占쎈윪甕곤옙
        fl_fclose( fp );

        // 占쎈쐻占쎈윥�젆袁��쐻占쎈윪甕곤옙 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�앗껊쐻占쎈윪�억옙 占쎈쐻占쎈짗占쎌굲占쎈쐻占쎈윪占쎄텑占쎈쐻占쎈윥壤쏉옙 占쎈쐻占쎈윪筌띻쑈�뇲占쎄땀�몭�쑙鍮녑뜝占� 占쎈쐻占쎈윪�얠±�쐻占쎈윥筌욑옙
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // 占쎈쐻占쎈윥占쎈꺊占쎈쐻占쎈윪�억옙 占쎈쐻占쎈윥筌묒궍�쐻占쎈윥獒뺤쉩�쐻占쎈윥�뜝�룞�삕 癲ル슪�삕占쎌넂�뜝�룞�삕筌묒궪�삕筌뤿떣�쐻�뜝占� 占쎈쐻占쎈윞占쏙옙占쎌녃域뱄퐦�삕濚밸㉡�삕
   // FlushFileSystemCache();
}

