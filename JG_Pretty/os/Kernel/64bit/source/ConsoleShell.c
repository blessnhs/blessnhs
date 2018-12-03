/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윞占쎄퐷占쎈쐻占쎈윞占쎄틩 �뜝�럥�맶�뜝�럥�쑅濾곌쑴占싸살맶�뜝�럥�쑅�뜝�럥援� 占쎈쐻占쎈윞占쎈뭼�뜝�럥�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윪雅뚢뫅�삕占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅占쎈뙀筌믠뫖�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕
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
// 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥占쎄샹占쎈쐻占쎈윥占쏙옙�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
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
//  �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑋�뜝�럩議� �뜝�럥�맶�뜝�럥�쑅濾곌쑴占싸살맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥鸚룐벂�쐻占쎈윥�걡�룞�삕占쎈젾�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럥�룈占쎈쨨�뜝占�
//==============================================================================
/**
 *  �뜝�럥�맶�뜝�럥�쑅濾곌쑴占싸살맶�뜝�럥�쑋�뵓怨ㅼ삕 �솾�꺂�뒧占쎈떔�뜝�럥�뵒占쎈쐻占쎈윥占쎈쑓 占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뮋�삕椰꾟뼹�쐻�뜝占�
 */

void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윞占쎄퐷占쎈쐻占쎈윞占쎄틩�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윞占쎈뭼�뜝�럥�맶�뜝�럥�몷占쎌뼔�꺋占쎌굲�뜝�럥�뿮占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쏙옙占쎈쐻占쎈윪占쎈츣占쎈쐻占쎈윥占쎌맽�뜝�럥�솑占썩뫀�뒭�뜝�뜦堉뱄옙�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�럸�뜝�럥利듸옙紐�獄�袁⑹굲�뜝�럡�꽟
    pstConsoleManager = GetConsoleManager();
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥裕잌뜝�럥�뿮�뜝�럥萸룟뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윞占쎄퐷占쎈쐻占쎈윞占쎄틩 �뜝�럥�맶�뜝�럥�쑅濾곌쒀�삕 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럡萸뉐뜝�럥�맶�뜝�럥�쑋�뜝�럡�뜲�뜝�럩�맳�뛾�룆캇占쎈뱥�뜝�럥�맶占쎈쐻�뜝占� TRUE�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�맱癒れ넂�뜝�룞�삕占쎈츧�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�럸�뜝�럥利듸옙紐�占쎌뵛占쎌굲雅��굢�삕
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윞占쎄퐷占쎈쐻占쎈윞占쎄틩 �뜝�럥�맶�뜝�럥�쑅濾곌쒀�삕 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럡萸뉐뜝�럥�맶�뜝�럥�쑋�뜝�럡�뜲�뜝�럩�맳�뛾�룆캇占쎈뱥�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥彛ュ뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�늾占쎈쇊占쎈엮占쎌몝占쎈쐻占쎈윥獒뺧옙 占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뮋�삕椰꾟댙�삕占쎌몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
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
                // �뜝�럥�맶�뜝�럥�쑋�뇦猿됰쭑占쎌맶�뜝�럥�쑋�뜝�럡�븤 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄у뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋嶺뚮씧�몚占쎈눚�뜝�럡��占쎈き占쎌몱占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�댇�뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� 占쎈쐻占쎈윥占쎈뼓癲ル슢�뀕占쎈쳮�뜝�럡猿� �뜝�럥�맶�뜝�럥�쑅嶺뚮씧�굫占쎌맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 占쎈쐻占쎈쑆泳�占썹뵓怨뺣쭠筌ｋ〕�삕占쎈폖�뜝�럩援뀐옙�쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡
                // 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �솾�꺂�뒧占쎈뎐�뜝�럩留녶뜝�럥�맶�뜝�럥吏쀥＄�룆�떢占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈뼓癲ル슢�뀕占쎈쳮�뜝�럡猿� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙�뜝�럥�맶�뜝�럥�쑋�뜝�럩議�
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
                // 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 �솾�꺂�뒧占쎈�뽳옙琉껓옙鍮띶뜝�럡�↓�얘퍜�삕占쎄뎡占쎈쐻占쎈윥�뤃占� �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑅嶺뚮쵓�삕
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥裕잌뜝�럥�뿮�뜝�럥萸룟뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙 �뜝�럥�럸�뜝�럥利멨뜝�럩援� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭占�
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙, CAPS Lock, NUM Lock, Scroll Lock�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥占쎈뼓占쎈쐻占쎈윥占쎄퉻占쎈쐻占쎈윥筌묕옙
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈쑆泳�占썹뵓怨뺣쭠筌ｋ〕�삕占쎈폖�뜝�럩援뀐옙�쐻占쎈윥占쎄괵�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럩踰곩뜝�럥�맶�뜝�럥�쑋�뜝�럡�꽟
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥�쑅�뜝�럥�뼀�뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�빊占썲뜝�럩肉ゅ뜝�럩援� �뜝�럥夷у뜝�럥利멨뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜏類㏃삕
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
 *  占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈ぁ占쎈꽠�ⓑ살┯占쎈뙀占쎌뵢占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥琉끻뜝�럥�맶�뜝�럥�쑅占쎌젂�뜝占� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뀎���맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // 占쎈쐻占쎈쑆泳�占썹뵓怨뺣쭠筌ｋ〕�삕占쎈폖�뜝�럩援뀐옙�쐻占쎈윥占쎄괵�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� 占쎈쐻占쎈윥鸚룐벂�쐻占쎈윥筌띲굩�삕占쎈엠占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥占쎄샹占쎈쐻占쎈윥占쏙옙�뜝�럡猿��뜝�럩�뮡�뛾占썲뜝占� �뜝�럥�늾占쎈쇊癰귨옙占쎄뎡�뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑅�뜝�럥琉끻뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲占쎌뼔�▽펺�뜉�삕泳��⑤┛�뜝占� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈꼥筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類ｋ쭫�빊占썲뜝�럩�뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꼫�뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚짹占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럡愿쒎뜝�럥�맶�뜝�럥�쑋�뜝�럩踰곩뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌븮�굝�눚�뜝�럡��占쎈き獄쏄퉭��占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅�뜏類ｋ쭫�빊占썲뜝�럩�뤈�뜝�럩援� �뜝�럥�늾占쎈쇊癰귨옙占쎄뎡�뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �솾�꺂�뒖占쎈１占쎌젂占쎈뼠占쎌굲�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�럞�뜝�럥�맶�뜝�럥�쑅占쎌젂熬곥끉�븸�슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쑋�뜝�럥泥� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쏙옙占쎈쐻占쎈윪占쎈츣占쎈쐻占쎈윥占쎌맽�뜝�럥�솑占썩뫀�뒭�뜝�뜦堉뱄옙�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭占�
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  占쎈쐻占쎈쑆泳�占썹뵓怨뺣쭠筌ｋ〕�삕占쎈폖�뜝�럩援뀐옙�쐻占쎈윥占쎄괵�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� 占쎈쐻占쎈윥鸚룐벂�쐻占쎈윥筌띲굩�삕占쎈엠占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占썹몴怨멸뎡占쎈쐻占쎈윥�댆占� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쏉옙�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�럸�뜝�럥利듸옙紐�獄�袁⑹굲�뜝�럡�꽟
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾嶺뚮엪�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙援욆떋紐뚯삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�럞�뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭븀빊占썲슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥利쇔뜝�럥夷у뜝�럥利멨뜝�럩援�
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊됬빊占썲뜝�럩逾��뜝�럥裕∽옙�눇�뙼蹂��굲 �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑅占쎈뎨�뜝�룞�뵾占쎌뒧占쎈떔�뜝�럩源볟뜝�럥鍮앾옙�쐻�뜝占� 占쎈쐻占쎈쑆泳�占썹뵓怨뺣쭠筌ｋ〕�삕占쎈폖�뜝�럩援뀐옙�쐻占쎈윥�뤃占� �뜝�럥�늾占쎈쇊癰귨옙占쎄뎡�뜝�럥�맶�뜝�럥�쐾�뜝�럥猷�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援욃뜝�룞�삕�뜝�럩援꿨뜝�럥��占쎈쐻占쎈짗占쎌굲�뜝�럥爾잌뜝�럩援� �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�럸�뜝�럥利듸옙紐�獄�袁⑹굲�뜝�럡�꽟
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占썹몴怨멸뎡占쎈쐻占쎈윥�댆占� �뜝�럥�맶�뜝�럥�쑋嶺뚮씧�몚占쎈눚�뜝�럡�븫�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥���뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅�뛾占썲뜝占�
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뀎���맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럥�룈占쎈쨨�뜝占�
//==============================================================================
/**
 *  �뜝�럥�맶�뜝�럥�쑅濾곌쒀�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚯빓�뿥占쎌맶�뜝�럥吏쀥뜝�럩援꿰솾�꺂�뒧占쎈뎐占쎌뵗占쎈쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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

    // �뜝�럥夷у뜝�럥利멨뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘 �뜝�럥���뜝�럥흮占쎄뎡 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄わ옙�쐻占쎈윥占쎌젋�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈쑆筌륁슱�삕泳�誘�踰㏆옙援�占쎌굲
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // �뜝�럥�맶�뜝�럥�쑅嶺뚯빓�뿥占쎌맶�뜝�럥吏쀥뜝�럩援꿰솾�꺂�뒧占쎈역�뜝�럩援� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // �솾�꺂�뒧占쎈�덃납占쏙쭜�뫀萸꾢뜝�럥�렂�뜝�럩援뀐옙�쐻占쎈윥占쎄샹 �솾�꺂�뒧占쎈뎐�뜝�럩瑗삼옙�굲�뛾占썲뜝占� �뜝�럥�늾占쎈쇊占쎈엮占쎌몝占쎈쐻占쎈윥獒뺧옙 �뜝�럥�맶�뜝�럥�쐾�뜝�럥利쇔뜝�럥�맶�뜝�럥�쑅�뜝�럥堉곩뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럩�뀇�윜諭꾪맊占쎄퉻�뜝�럡�뜦�뜝�럡�옖�맱袁ъ삕占쎌맶占쎈쐻�뜝占�
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
 *  �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볩옙�쐻占쎈윥�뤃占� �솾�꺂�뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
 */
static void Cls( const char* pcParameterBuffer )
{

    // �솾�꺂�뒧占쎈역�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋嶺뚮엪�삕嚥싳쉶瑗ч뇡癒�爾��뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�꺐占썩뫀�뒩泳��겭�쐻占쎈윥�뙴占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�뜲踰㎫뙼�뿪�꽫�뜝�럩援꿨뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볩옙�쐻占쎈윥�뤃占� �솾�꺂�뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럥占쏙옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럩留�, �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌뼲�삕 1�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄у뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉻�삕
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // �솾�꺂�뒧占쎈역�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋嶺뚮엪�삕嚥싳쉶瑗ч뇡癒�爾��뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�꺐占썩뫀�뒩泳��겭�쐻占쎈윥�뙴占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�뜲踰㎫뙼�뿪�꽫�뜝�럩援꿨뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볩옙�쐻占쎈윥�뤃占� �솾�꺂�뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럥占쏙옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럩留�, �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌뼲�삕 1�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄у뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉻�삕
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
	// �솾�꺂�뒧占쎈역�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋嶺뚮엪�삕嚥싳쉶瑗ч뇡癒�爾��뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�꺐占썩뫀�뒩泳��겭�쐻占쎈윥�뙴占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�뜲踰㎫뙼�뿪�꽫�뜝�럩援꿨뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볩옙�쐻占쎈윥�뤃占� �솾�꺂�뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럥占쏙옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럩留�, �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌뼲�삕 1�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄у뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉻�삕
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
 *  占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �솾�꺂�뒧占쎈떔�뜝�룞�삕占쎈쐻占쎈윞�굜�깷�쐻占쎈윥占쎈젗�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈뼀占쎌뒏�젆�몼�맀筌뤿굢�삕占쎈섰占쎌맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럡�븤�뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑋�뜝�럡�꺘(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎌몱占쎌맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎈뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑋雅��겇猷몄떪占쎌넂占쎄낯援�
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
    // �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕占쎈뀇�윜諭꾪맔占쎌굲占쎈쎗�뜝�뜴�쐻�뜝占� 占쎈쐻占쎈윥占쎈뮇力놂옙域밟뫁�굲�뛾占쏙옙萸울옙�뮋�뜝�럥�꽎�뜝�럡肄わ옙�쐻占쎈윪筌륁±�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈빝野껋눖�맶�뜝�럥�쑅�뜝�럥琉� PC占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럡�븤�뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑋�뜝�럡�꺘
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛볟뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅占쎈뙀占싸살맶�뜝�럥�쑅嶺뚯빖占쎈Ŋ�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �솾�꺂�돲占쏙옙�뙴湲룸쐻占쎈윪占쎌졂
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑅�뜝�럥�뼓 �뜝�럩�뀇�윜諛잙쳛占쎄뎡�뜝�럥�맶�뜝�럥�쑋�뜝�럡���뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾�뜝�럥鍮ｅ뜝�럥�맶�뜝�럥�쑅�뜝�럥萸� �뜝�럥�눚�뜝�럡�뀷�뜝�럩源볩옙�쐻占쎈윥獒뺛룇�삕占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛볟뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅占쎈뙀占싸살맶�뜝�럥�쑅嶺뚯빖占쎈Ŋ�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥夷у뜝�럥利꿨뜝�럥爾�占쎈쐻占쎈윪占쎌젦�뜝�럥�맶�뜝�럥�쑋�뜝�럩諭얍뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �솾�꺂�돲占쏙옙�뙴湲룸쐻占쎈윪占쎌졂
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲�뜝�럥瑗ε뜝�럥�맶占쎈쐻�뜝占� �뜝�럩�뀇�윜諭꾪맊�떋�떣�쐻占쎈윥占쎈㎟
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC 占쎈쐻占쎈윥占쎈뮇力놂옙域밟뫁�굲�뛾占쏙옙萸울옙�뮋�뜝�럥�꽎�뜝�럡肄わ옙�쐻占쎈윪筌륁쥜�삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘�뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뜝�럡猿� �뜝�럥�럸�뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC 占쎈쐻占쎈윥占쎈뮇力놂옙域밟뫁�굲�뛾占쏙옙萸울옙�뮋�뜝�럥�꽎�뜝�럡肄わ옙�쐻占쎈윪筌륁쥜�삕占쎌맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�럸�뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럥占썩댙�삕占쎌맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅�뜝�럥六섇뜝�럥�맶�뜝�럥�쑋嶺뚮씧��占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럩�뀇�윜諛잙쳛占쎄뎡�뜝�럥�늾占쎈쇊癰귨옙占쎄뎡
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럥占썩댙�삕占쎌맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅�뜝�럥六섇뜝�럥�맶�뜝�럥�쑋嶺뚮씧��占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럩�뀇�윜諛잙쳛占쎄뎡�뜝�럥�늾占쎈쇊癰귨옙占쎄뎡
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
 *  �뜝�럥�맶�뜝�럥�쑋�뇦猿됰쭑占쎌맶�뜝�럥�쑋�뜝�럡�븤 �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾�뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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
    
    // 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾤솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뮋�삕椰꾟댙�삕占쎌몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚯빘�눜�빊占썲슖�뼯爰귨옙源볟뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돸占쎌굲�뜝�럥爰뗥뜝�럩援꿩퓴�쉶瑗ч뇡癒�爾ο옙�쐻占쎈윪筌륁쥜�삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥彛у뜝�럥�맶�뜝�럥�쑅嶺뚮엪�삕
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖�뜝�럥夷у뜝�럥利멨뜝�럩援� 2�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾嶺뚮씭占쏙퐢�맶�뜝�럥�쑋占쎌뼚伊됬빊占썲슖�볥걙占쎄뎡 �뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돸占쎌굲�뜝�럥爰뗥뜝�럩援꿩퓴�쉶瑗ч뇡癒�爾ο옙�쐻占쎈윪筌륅옙 �뜝�럩�뀇�윜諭꾩삕�뜝�럥爾η튊踰우슦�굲 �뜝�럥夷у뜝�럥利든뜏類ｋ룏占쎌굲�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    if( iProcessorCount > 1 )
    {
        // �뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돸占쎌굲�뜝�럥爰뗥뜝�럩援꿩퓴�쉶瑗ч뇡癒�爾ο옙�쐻占쎈윪筌륅옙 �뜝�럩�뀇�윜諭꾩삕�뜝�럥爾η튊踰우슦�굲 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥夷у뜝�럥利든뜏類ｋ룏占쎌굲�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈쑆泳�占썹뵓怨ㅼ삕�뜝�럥�뒩占쎈빃�뜝�럩援뀐옙�쐻占쎈윥�뤃占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럩諭삣뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅�뜏類ㅼ돧占쎌럸�뜝�럥利닷뜝�룞�삕濚욌꼬�슦�굲 �솾�꺂�뒱占쎌굲占쎈꽠�⑨퐢爾밧뜝�럩援�
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
        // TCB占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥鸚룐벂�쐻占쎈윥�굢�꺈�삕占쎈쨫�뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� TCB�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢� 嚥싳쉶瑗э옙沅∽옙爰랃옙�쐻占쎈윥占쎄샹�솾�꺂�뒧占쎈벨�삕占쎄뎡 ID占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒉筌뚮냵�삕�뙴�뵃�삕占쎄뎡 6�뜝�럥夷у뜝�럥利멨뜝�럩援� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�빊占썲뜝�럩逾��뜝�럩留꾬옙�쐻占쎈윥占쎈젩, 占쎈쐻占쎈쑆筌륁슱�삕泳�誘ｋ쐻占쎈윞占쎈뼏 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅嶺뚮씭留잏빊占썲뜝�럩�뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�뿼占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥�몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꼫�뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕
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
 *  �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙�뿺占쎌맶�뜝�럥�쑋�뜝�럩�졂 ID�솾�꺂�뒧占쎈역�뜝�럩援� 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮뽳옙�떚�뜝�럩援꿨뜝�럥��占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�늾占쎈쇊占쎈엮占쎌몝占쎈쐻占쎈윥獒뺧옙
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥筌β뫜�삕占쎌맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럩議녶뜝�럥�맶�뜝�럥�쑋�뜝�럥�닞
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
    // 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윞占쎄퐷占쎈쐻占쎈윞占쎄틩 �뜝�럥�맶�뜝�럥�쑅濾곌쑴占쏙옙占쎌굲�뜝�럥爾잌뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑋嶺뚮쪋�삕 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩議녶뜝�럥�맶�뜝�럥�쑋�뜝�럥�닞�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀�뜝占� 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥筌β뫜�삕占쎌맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙�뜝�럥�맶�뜝�럥�쑋�뜝�럩議� �솾�꺂�뒧占쎈�덃납占쏙쭜�뫀萸꾢뜝�럥�렂�뜝�럩援꿨뜝�럡�뜦�뜝�럥�렂�뜝�럩援꿨뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩議녶뜝�럥�맶�뜝�럥�쑋�뜝�럥�닞
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
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛볟뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢쏉옙�쐻占쎈윥占쎄괴�뜝�럥苑욃슖�댙�삕�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // �뜝�럥夷у뜝�럥利멨뜝�럩援� 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 �뜝�럩�뀇�윜諭꾩삕�뜝�럥爾η튊踰우슦�굲 占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥��占쎈쐻占쎈짗占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈쑆泳�占썹뵓怨ㅼ삕�뜝�럥�뒩占쎈빃�뜝�럩援뀐옙�쐻占쎈윥�뤃占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럩諭삣뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅�뜏類ㅼ돧占쎌럸�뜝�럥利닷뜝�룞�삕濚욌꼬�슦�굲 �솾�꺂�뒱占쎌굲占쎈꽠�⑨퐢爾밧뜝�럩援�
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎈뵃�삕占쎌맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�럸�뜝�럥利든뜏類ｋ렱占쎈뼆占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕�젆�뫅�삕�솒占쏙옙援� �뜝�럥�맶�뜝�럥�쑋嶺뚮씧�맻占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럩�뀇�윜諛잙쳛占쎄뎡�뜝�럥�맶�뜝�럥�쑅�뜝�럥臾�
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎄뭍占쎌맶�뜝�럥�쑋�뵓寃꾨벉�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎈뵃�삕占쎌맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�럸�뜝�럥利듸옙紐�獄�袁⑹굲�뜝�럡�꽟
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  �솾�꺂�뒱占쎈쭓辱됰봿�땬占쎌굲�뜝�럡猿묕옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뵓寃꾨벉�맶�뜝�럥�쑋�뜝�럥泥볟뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥占쎌맆�뜝�럥荑� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占�
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // �뜝�럥�맶�뜝�럥�쑋�뜝�럡占썲뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕 �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥��耀붾굝梨멨뜝�떬�돃��占쎈쐻�뜝占�
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
 *  �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �솾�꺂�뒧占쎈턂�뜝�럡愿랃옙�쐻占쎈윥�굜占쏙옙�쐻占쎈윥占쎈젗�뜝�럥�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥筌ο옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볟뜝�럥�맗�윜諛잙쳛占쎄뎡占쎈쐻占쎈윪獄�占� �뜝�럩�뀇�윜諭꾪맊占쎄퉻�뜝�럡�뜦�뜝�럡�옪占쎈뼁占쎈쐻占쎈윥占쎈빝占쎈쐻占쎈윥筌ο옙 �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅�뜏類㏃삕
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

    // �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二룟뜝�럥�맶�뜝�럥�쑅�뜝�럥�룎�솾�꺂�뒧占쎈벨�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅�뜏類㏃삕 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
    GetCh();
}

/**
 *  �솾�꺂�뒧占쎈턂�뜝�럡愿랃옙�쐻占쎈윥�굜占쏙옙�쐻占쎈윥占쎈젗�뜝�럥�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥筌ο옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈떔�뜝�럩源볩옙�쐻占쎈윥�뤃占� �뜝�럩�뀇�윜諭꾪맊占쎄퉻�뜝�럡�뜦�뜝�럡�옖�맱袁ъ삕占쎌맶占쎈쐻�뜝占�
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒉筌뚮냵�삕�뙴�뵃�삕占쎄뎡 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲 �뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�맱癒れ넂�뜝�룞�삕占쎈츧�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥���뜝�럥흮占쎄뎡
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
 *  �뜝�럥�맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑋�뜝�럩諭� �솾�꺂�뒧占쎈떔�뜝�룞�삕占쎈쐻占쎈윞�굜�깷�쐻占쎈윥占쎈젗�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
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
 *  �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // �솾�꺂�뒧占쎈�덃납占썲뜝�뜴�쐻占쎈윪筌륅옙 �뜝�럥�꺐占썩뫀踰좑옙留뉛옙�쐻占쎈윞占쎈룺 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅わ옙�굲占쎌뼔�꺋占쎌굲�뜝�럥�뒇�뜝�럡�댉 �뜝�럥�꺐占썩뫀踰좑옙留뉛옙�쐻占쎈윞占쎈룺 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥�벍�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占�, �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곎딆굲占쎌뼔�쎾뜝�뜦維�甕곗쥜�삕筌랃옙, �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곎딆굲占쎌뼔�쎾뜝�뜦維�甕곗쥜�삕筌랃옙 �뜝�럥�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // 占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�꺊�굢�맮�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥彛녶뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑋雅��굢�삕 LBA �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� �솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
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
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭�뫅�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 LBA �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾤솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 �솾�꺂�뒧占쎈떔�뜝�룞�삕占쎈쐻占쎈윞�굜�깷�쐻占쎈윥占쎈젗占쎈쐻占쎈윥筌랃옙�뜝�럩援꿨뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�럸�뜝�럥利듸옙�쇀占쎈뢾占쎈�占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥���뜝�럥흮占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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

                // �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럡肄� �뜝�럥�맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쎈젗占쎈쐻占쎈윥筌띯뮪�뀋�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二놂옙�쐻占쎈쑆泳��굢�삕占쎄뎡 16�뜝�럩�뀇�윜諭꾪맔占쎌굲占쎈쐻占쎈윥占쎈젩 �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꺘�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�늾占쎈쇊占쎈엮占쎌몝占쎈쐻占쎈윥獒뺧옙 0�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜝�럥琉끾퓴�쉶瑗э옙�뒭占쎄뎡
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
 *  �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�꺊�굢�맮�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥彛녶뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑋雅��굢�삕 LBA �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� �솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
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

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭�뫅�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 LBA �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
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
    
    // �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�酉대쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�럸�뜝�럥利듸옙�쇀占쎈뢾占쎈�占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �솾�꺂�뒱占쎌굲占쎈꽠�⑨퐢爾밧뜝�럩援�.
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥猷덂뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 4 �뜝�럥�럸�뜝�럥利닷뜝�럥�뵒占쎈쐻占쎈윥占쎄샹�뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑋�뵓怨ㅼ삕 LBA �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥吏쀥뜝�럩援� 4 �뜝�럥�럸�뜝�럥利닷뜝�럥�뵒占쎈쐻占쎈윥占쎄샹�뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅占쎈뼀�뇡占썲젆�몼�맀筌뤿굢�삕占싸살맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮ケ猿딆맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // �뜝�럥�맶�뜝�럥�쑅占쎈뼀�뇡占썲젆�뫅�삕�솒占쏙옙援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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

            // �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럡肄� �뜝�럥�맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쎈젗占쎈쐻占쎈윥筌띯뮪�뀋�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二놂옙�쐻占쎈쑆泳��굢�삕占쎄뎡 16�뜝�럩�뀇�윜諭꾪맔占쎌굲占쎈쐻占쎈윥占쎈젩 �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꺘�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�늾占쎈쇊占쎈엮占쎌몝占쎈쐻占쎈윥獒뺧옙 0�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜝�럥琉끾퓴�쉶瑗э옙�뒭占쎄뎡
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
 *  �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�삓�뜝�럥�늾占쎈쇊癰귨옙占쎄뎡
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
 *  �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈뼢占쎌뒭占쎄뎡占쎈쐻占쎈윥�뤃占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾(�뜝�럥�맶�뜝�럥�쑅櫻뗫봿占썬룂�븸�뜝�럩肉ゅ뜝�럩援�)
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
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
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
 *  占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뭿�쐻占쎈윥�굜占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑋�뜝�럩�쓥�뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼲�삕獄�占� 占쎈쐻占쎈윥占쎈ぁ�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
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

	// �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뭿�쐻占쎈윥�굜占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑋�뜝�럩�쓥�뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼲�삕獄�釉앹삕占쎌맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙�뜝�럥�맶�뜝�럥�쑋�뜝�럩議�
 */
static void DeleteFile( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뭿�쐻占쎈윥�굜占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑋�뜝�럩�쓥�뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�슕由겼뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �솾�꺂�뒧占쎈�덃납占쏙쭜�뫀萸꾢뜝�럥�렂�뜝�럩援뀐옙�쐻占쎈윥�뤃占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕占쎈뀇�윜諭꾪맔占쎌굲占쎈쎗�뜝�룞�삕占쎈ご�뜝�럥�뱺�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二룟뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭�뫅�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲占쎌뼔�▽펺�뜉�삕泳��ⓦ궘�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�삓�뜝�럥�맶�뜝�럥�쑅占쎈뙀占싸살맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� 3�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�돫�뜝�럥�맶�뜝�럥�쑋�뜝�럥泥볡솾�꺂�뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�맱癒れ넂�뜝�룞�삕占쎈츧�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕占쎌맶�뜝�럥�쑋占쎌뼚伊됬빊占썲슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�삓�뜝�럥�맶�뜝�럥�쑅占쎈뙀�뜝占� 3�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�돫�뜝�럥�맶�뜝�럥�쑋�뜝�럥泥볟뜝�럥�솑占썩뫀踰∽옙裕쏉옙�쐻占쎈윥筌λ〕�삕筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꼫�뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 占쎈쐻占쎈윥占쏙옙占쎈쐻占쎈윥�댚�뮋�삕椰꾟댙�삕占쎌몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈ぁ�뜝�럥苑욃슖�댙�삕�썒�슪�삕 �뜝�럥�맶�뜝�럥�쐾�뜝�럥利쇔뜝�럥夷у뜝�럥利멨뜝�럩援�
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�뼀�뜝�럥�맶�뜝�럥�쑅占쎈쑏占쎌뵢占쎌맶�뜝�럥�쑋占쎌븮�굞�븸�슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙 �뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭占�
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
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭�뫅�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲占쎌뼔�▽펺�뜉�삕泳��ⓦ궘�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾占쎈쨨�뜝�뜽�벖獄쏅ℓ占썲뜝�럥裕앭뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�늾占쎈쇊占쎈㎦占쎌맠占쎈쐻占쎈윥�뤃占� �뜝�럥�럸�뜝�럥利듸옙紐�占쎌뵛占쎌굲雅��굢�삕
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // �솾�꺂�뒧占쎈뎐�뜝�럥裕앭뜝�럥�몡占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�삕占쎌맶�뜝�럥�쑋占쎌뼚伊됬빊占썲슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥占썲뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙옙 �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �솾�꺂�뒩�뜮�꼳�쑏癰귥옓�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥筌묕퐦�삕占쎌맶�뜝�럥�쐾�뜝�럥占쎌쉩�쐻占쎈쑆泳��굢�삕占쎄뎡 20�뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌븪猷몄떪占쎌넂�뜝�룞�삕占쎈츧�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅嶺뚮ギ�벉�맶�뜝�럥�쑅占쎌젂熬곥끉�븸�슖�볥걙占쎄뎡
        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥彛� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅嶺뚮씭留잏빊占썲뜝�럩�뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�뿼占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥�몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈뼓占쎈쐻占쎈윥占쎈빢�뜝�럩�쟼濚밸Þ�궖�꽴�슪�삕占쎌맶占쎈쐻�뜝占�
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
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈뼢占쎌뒭占쎄뎡占쎈쐻占쎈윥�댆占� �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵓�삕 �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
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
 *  �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅わ옙�굲占쎌뼔�꺋占쎌굲�뜝�럥�뒇�뜝�럡�댉 �뜝�럥�맶�뜝�럥�쑅櫻뗫봿占싸살맶�뜝�럥�쑅�뛾占쏙옙萸울옙�뮋�뜝�럥猷긺뜏類ｋ�占쎌맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿뗫뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘
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
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앾옙�쐻�뜝占� 占쎈쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼍吏놅옙�돥占쎌맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥塋딅ㅇ�쐻占쎈윥占쎈쭇�뜝�럥�렓占썩벀�걠占쎄뎡占쎈쐻占쎈윥�몭�뫅�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲占쎌뼔�▽펺�뜉�삕泳��ⓦ궘�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
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
    
    // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅わ옙�굲占쎌뼔�꺋占쎌굲�뜝�럥�뒇�뜝�럡�댉 �뜝�럥�맶�뜝�럥�쑅櫻뗫봿占싸살맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑋�뵓怨ㅼ삕 FIFO占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 占쎈쐻占쎈윥占쎈ぁ占쎈꽠�⑨퐢爾밧뜝�럩援�
    ClearSerialFIFO();
    
    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚횞筌뚮냵�삕�뙴�뵃�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎈뼣�맱癒れ넂�뜝�룞�삕占쎈츧�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥��耀붾굝梨멨뜝�뜦�뙔占쎌굲占쎌젂熬곎딆굲占쎌뼔�쎾뜝�뜦維�甕곗±�젂熬곻옙占쎌맶�뜝�럥�쑅�뜏類㏃삕 �솾�꺂�뒧占쎈떔�뜝�럥�뵒占쎈쐻占쎈윥筌묕퐦�뵾占쎌뒩占쎈뤈�뜝�럩援뀐옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡, 4 �뜝�럥�럸�뜝�럥利닷뜝�럥�뵒占쎈쐻占쎈윥占쎄샹�뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚋우삕
    // Ack占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩踰곩뜝�럥�맶�뜝�럥�쑅占쎈빝�뜝占�
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾤솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�럞�뜝�럥�맶�뜝�럥�쑅占쎌젂熬곥끉�븸�슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑋�뜝�럡占썲뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕 �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥���뜝�럥흮占쎄뎡
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥��耀붾굝梨멨뜝�떬�돃��占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴袁ъ삕泳�誘ｋ쐻占쎈윥占쎄샹 30占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾嶺뚮씭占쏙퐢�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑋占쎌븮�굞�븸�슖�볥걙占쎄뎡 Time Out�뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� 嚥싳쉶瑗э옙沅∽옙爰띶뜝�럥�맶占쎈쐻�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // �솾�꺂�뒧占쎈뎐�뜝�럩留녶뜝�럥�맶�뜝�럥吏쀥＄�룆踰⑨옙�굲嶺뚮씚維쒙옙�굲嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴袁ъ삕泳�誘ｋ쐻占쎈윥�뤃占� �뜝�럥夷у뜝�럥利꿨뜝�럥猷띰옙�쐻占쎈윥�젆占�
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럥�맶�뜝�럥�쐾嶺뚮씭占쏙퐢�맶�뜝�럥�쑋�뜝�럩諭얍뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅嶺뚮ギ�벉�맶�뜝�럥�쑋嶺뚮ㅏ諭룩떋�띿삕�몭�궪�삕占쎄뎡�뜝�럩�뮋�뜝�럥�솕�뜝�럩援�, Ack占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈빝占쎈꺽占쎌맶�뜝�럥�쑅嶺뚮쵓�삕
    SendSerialData( "A", 1 );

    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅わ옙�굲占쎌뼔�꺋占쎌굲�뜝�럥�뒇�뜝�럡�댉�뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎌맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘
    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // �뜝�럥�꺐占썩뫀�뒩泳�蹂잙쐻占쎈윪獄�釉앹삕占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅占쎌젂筌믠뫖�맶�뜝�럥�쑅�뜝�럥�뼀�뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊됮�얜퀬占썩뫀踰좑옙留녶뜝�럡�뜦占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�늾占쎈쇊占쎈㎦占쎌맠占쎈쐻占쎈윥占쎄샹 �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅占쎌젂熬곥끉�븸�슖�볥걙占쎄뎡 ACK �뜝�럥�맶�뜝�럥�쑋占쎈쨨占쎄퐩占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅占쎈뼀�뇡占썲젆�뫅�삕�솒占쏙옙援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
        if( dwTempSize != 0 ) 
        {
            // �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �솾�꺂�뒩占쎌굨�뜝�럩�몝�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �솾�꺂�뒧占쎈뎐�뜝�럩留녶뜝�럥�맶�뜝�럥吏쀥＄�룆踰⑨옙�굲嶺뚮엪�삕占쎈쐻占쎈윥筌뚳옙�솾�꺂�뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅嶺뚮ギ�쑑占썹넭怨λ즸�뜝�뜦�꼥筌뚭퍌�쐻�뜝占� FIFO�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾占쎈뼀占쎌뒏�젆�몼�맀筌뤿굢�삕影�瑜곸굲占쎌뼲�삕
            // 16 �뜝�럥�럸�뜝�럥利닷뜝�럥�뵒占쎈쐻占쎈윥占쎄샹�뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙裕곭빊占썲뜝�럩逾��뜝�럩留꾬옙�쐻占쎈윥占쎈젩 �뜝�럥�맶�뜝�럥�쑅占쎈뎨占쎈쑟�얜퀬占썩뫀踰좑옙留녶뜝�럩堉뀐옙�굲 Ack占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩踰곩뜝�럥�맶�뜝�럥�쑅占쎈빝�뜝占�
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // �뜝�럥�맶�뜝�럥�쑅占쎈뼀�뇡占썲젆�뫅�삕�솒占쏙옙援� 嚥싳쉶瑗э옙沅∽옙爰띶뜝�럡�뜦占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈뼓癲ル슢�뀕占쎈쳮�뜝�럩議녶뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롪퀡��耀붾굝梨멨뜝�뜦�뙔占쎌굲�뜝�럥諭� �뜝�럥�럸�뜝�럥利닷뜝�룞�삕濚욌꼬�슦�굲 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // �솾�꺂�뒧占쎈뎐�뜝�럩留녶뜝�럥�맶�뜝�럥吏쀥＄�룆踰⑨옙�굲嶺뚮씚維쒙옙�굲嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴袁ъ삕泳�誘ｋ쐻占쎈윥�뤃占� �뜝�럥夷у뜝�럥利꿨뜝�럥猷띰옙�쐻占쎈윥�젆占�
            qwLastReceivedTickCount = GetTickCount();
        }
        // �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊됮�얜퀬占썩뫀踰좑옙留녶뜝�럡�뜦占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�럞�뜝�럥�맶�뜝�럥�쑅占쎌젂熬곥끉�븸�슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑋�뜝�럡占썲뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕 �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥���뜝�럥흮占쎄뎡
        else
        {
            Sleep( 0 );
            
            // �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥��耀붾굝梨멨뜝�떬�돃��占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅륅쭔�냵�삕�뙴袁ъ삕泳�誘ｋ쐻占쎈윥占쎄샹 10占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾嶺뚮씭占쏙퐢�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑋占쎌븮�굞�븸�슖�볥걙占쎄뎡 Time Out�뜝�럥�맶�뜝�럥�쑋嶺뚮ㅏ諭띰옙�뮋�뜝�럥�솕�뜝�럩援� 嚥싳쉶瑗э옙沅∽옙爰띶뜝�럥�맶占쎈쐻�뜝占�
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑋�뜝�럩踰곭솾�꺂�뒱占쎈쭟�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾占쎈뼀占쎌뒏�젆�몼�맀筌뤿굢�삕影��굝�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑋�뜝�럩議녶뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕 �뜝�럥�럸�뜝�럥利듸옙�쇀占쎈렰占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쐾占쎈뼀占쎌뒏�젆�몼�맀筌뤿굢�삕占쎈섰占쎌맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥占쎈ぁ占쎈꽠�ⓑ살┯占쎈뙀占쎌뵢占쎌맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜝�럥�뿼 �뜝�럥�맶�뜝�럥�쐾�뜝�럥�젾占쎈쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�뿼占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥�몷嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占�
    // 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚋우삕, �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂占쎌뿪占쎌굲�뜝�럥爾잌뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵐沅わ옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 占쎈쐻占쎈윥占쎈ぁ占쎈꽠�⑨퐢爾밧뜝�럩援�
    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑅嶺뚮쵐�뒅占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥���뜝�룞�삕嶺뚳옙筌뚭쑴�굲占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈ぁ占쎈꽠�ⓑ살┯占쎈뙀占쎌뵢占쎌맶�뜝�럥�쑅�뜝�럥琉끻뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� 占쎈쐻占쎈윥占쎈뼓癲ル슢�뀕占쎈쳮�뜝�럩議녶뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�럸�뜝�럥利든뜏類ｋ렱占쎈뼆占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅嶺뚮ギ�벉�맶�뜝�럥�쑅�뜏類ｋ쭫�빊占썲뜝�럩�뤈�뜝�럩援뀐옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂占쎌뿪占쎌굲�뜝�럥爾잌뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵐沅わ옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럩�뀇�윜諭꾪맔占쎌굲嚥싲갭�돘占쎌굲
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP �뜝�럥�맶�뜝�럥�쐾�뜝�럥彛ュ뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂 �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥占쎄샹占쎈쐻占쎈윥占쏙옙�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� I/O APIC�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뜝�럥占썲뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑋占쎄덩�뜝�룞�삕占쎌맶�뜝�럥�쑋�뜝�럩二� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援�(INTIN)�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 占쎈쐻占쎈윞占쎈뭼�뜝�럥�맶�뜝�럥�몶占쎈쐻占쎈쑆筌륁슱�삕泳��뵃�삕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥占쎄샹占쎈쐻占쎈윥占쏙옙�뜝�럡猿��뜝�럩�뮡�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡愿닷뜝�럥�맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윞占쎈뭼�뜝�럥�맶�뜝�럥�몷占쎌뼔�꺋占쎌굲�뜝�럥�뿮占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑋�뜝�럡猿묕옙�쐻占쎈윥占쏙옙占쎈쐻占쎈윪占쎈츣占쎈쐻占쎈윥占쎌맽�뜝�럥�솑占썩뫀�뒭�뜝�룞�맅�뤃�꽒�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙 �뜝�럥�맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋濚밸Ŧ�쐣占쎌굲占쎈쑏占쎈꺽占쎌맶占쎈쐻�뜝占�
    PrintIRQToINTINMap();
}

/**
 *  占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 �뜝�럩�뀇�윜諭꾩삕�뜝�럥爾η튊踰우슦�굲 �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뜝�럥占썲뜝�럥�맶�뜝�럥�쑅�뛾占쏙옙裕뗰옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뀎���맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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
    
    // MP �뜝�럥�맶�뜝�럥�쐾�뜝�럥彛ュ뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂 �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥占쎄샹占쎈쐻占쎈윥占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럥援� �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘�뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥夷у뜝�럥利든뜏類ｋ룏占쎌굲�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    //==========================================================================
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛볟뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾤솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 Column�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    // �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� 嚥싳쉶瑗ч뇡癒�爾��뜝�럡�뜦占쎈쐻�뜝占� 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 4�뜝�럥夷у뜝�럥利든뜏類ｋ뢾�뜝�럩堉뀐옙�굲 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� Column�뜝�럥�맶�뜝�럥�쑅占쎌젂�뜝占� 15�뜝�럥�눚�뜝�럡�뀷嶺뚳옙筌뚭쑴�굲�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂影��굝�맶�뜝�럥�쑅嶺뚮엪�삕
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
        
        // 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈쑆泳�占썹뵓怨ㅼ삕�뜝�럥�뒩占쎈빃�뜝�럩援뀐옙�쐻占쎈윥�뤃占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럩諭삣뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎈뮋�뜝�럥�솕�뜝�럩援� �솾�꺂�뒱占쎌굲占쎈꽠�⑨퐢爾밧뜝�럩援�
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뜝�럥占썲뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    //==========================================================================
    // 占쎈쐻占쎈윥塋딅쪋�삕占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뜝�럥占썲뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥吏쀥뜝�럩援� 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 �뜝�럩�뀇�윜諛잙쳛占쎄뎡 �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥�맶�뜝�럥�쑋�뜝�럥占썲뜝�럥�맶�뜝�럥�쑅�뛾占썲뜝占� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럥痢녶뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙, �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� 嚥싳쉶瑗ч뇡癒�爾��뜝�럡�뜦占쎈쐻�뜝占� 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럩�뮔�뜝�럡�맖 4�뜝�럥夷у뜝�럥利든뜏類ｋ뢾�뜝�럩堉뀐옙�굲 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝占� Column�뜝�럥�맶�뜝�럥�쑅占쎌젂�뜝占� 15�뜝�럥�눚�뜝�럡�뀷嶺뚳옙筌뚭쑴�굲�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占�
            if( j == 0 )
            {
                // 20 �뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌븪猷밸븸�뜝�럩逾��뜝�럩留꾬옙�쐻占쎈윥占쎈젩 �뜝�럥�맶�뜝�럥�쑋�뜝�럡���솾�꺂�뒧占쎈벨�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�솾�꺂�뒩占쎈뤈�뜝�럩援�
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
            // 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 �뜝�럥�맶�뜝�럥�쐾�뜝�럥占쏙퐦�삕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뇦猿곴뭍占쎌맶�뜝�럥�쑅�뜝�럥���뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �솾�꺂�뒧占쎈�덃납占썲뜝�뜾異��몴�굞�굲 �뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럩諭삣뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎈뮋�뜝�럥�솕�뜝�럩援� �솾�꺂�뒱占쎌굲占쎈꽠�⑨퐢爾밧뜝�럩援�
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
 *  �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�눚�뜝�럡���뜏類ｋ㎜占쎌굲�뜝�럡���뜝�럥�맶�뜝�럥�쑅嶺뚯빖占쎈Ŋ�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럩�뀇�윜諛잙쳛占쎄뎡�뜝�럥�늾占쎈쇊癰귨옙占쎄뎡
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀�뜝占� ID �뜝�럥�맶�뜝�럥�쑅�뜝�럥�뼀�뜝�럥�맶�뜝�럥�쑅占쎈쨨�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類ｋ�占쎈튉占쎈쐻占쎈쑕占쎌맶�뜝�럥�쐾�뜝�럥彛� �뜝�럥�눚�뜝�럡���뜏類ｋ㎜占쎌굲�뜝�럡���뜝�럥�맶�뜝�럥�쑅嶺뚯빢�삕(Affinity) 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
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
 *  VBE �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諛잙쳛占쎄뎡 占쎈쐻占쎈윥占쏙옙�뜝�럡猿��뜝�럥�룒勇싲쨪�삕�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諛잙쳛占쎄뎡 占쎈쐻占쎈윥占쏙옙�뜝�럡猿��뜝�럥�룒勇싲쨪�삕�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�럸�뜝�럥利듸옙紐�獄�袁⑹굲�뜝�럡�꽟
    pstModeInfo = GetVBEModeInfoBlock();
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥琉끻뜝�럥�맶�뜝�럥�쐾嶺뚮씭占쏙퐢�맶�뜝�럥�쑅嶺뚯빓�뿥占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�졂�뜝�럩�뀇�윜諭꾪맔占쎌굲�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋嶺뚮씧�뫁占쎈쐪�뜝�럩�뮝�뜝�럥�빝濚욌꼬�슦�굲 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곤옙
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
 *  �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 占쎈쐻占쎈윞占쏙옙占쎈쐻占쎈윥筌λ뿥�쐻占쎈윥�뤃占� �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈윥�뜝�룞�삕�뜝�럩援뀐옙�쐻占쎈윥筌β뫜�삕占쎌맶�뜝�럥�쑅�뛾占쏙옙萸울옙�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑋占쎈쐻占쎈쑕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // �뜝�럥�맶�뜝�럥�쑅嶺뚯쉸占싸살맶�뜝�럥�쑋�뜝�럩諭� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�맶�뜝�럥�쑋�뇦猿곴뭍占쎌맶�뜝�럥�쑅�뜝�럥���뜝�럥�맶�뜝�럥�쑅�뜝�럥援� 4Kbyte �솾�꺂�뒧占쎈떔�뜝�룞�삕占쎈쐻占쎈윞�굜�깷�쐻占쎈윥占쎈젗占쎈쐻占쎈윥筌랃옙�뜝�럩援꿨뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�럸�뜝�럥利듸옙�쇀占쎈뢾占쎈�占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎈쐻占쎈쑕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒻占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾�뜝�럥�맶�뜝�럥�쑅嶺뚮엪�삕 嚥싳쉶瑗э옙�뒭占쎄뎡占쎈쐻占쎈윥占쎈ぁ占쎈꽠�⑨퐢爾잌뜝�럩援꿨뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮엪�삕
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // �뜝�럥�맶�뜝�럥�쑋占쎈쐻占쎈쑕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎈뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�럥占싼띿삕占쎌맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅嶺뚮엪�삕 SystemCallTestTask() �뜝�럥�맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅�뜝�럥臾꾢뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 占쎈쐻占쎈윞占쏙옙�뜝�럡猿��뜝�럥�룈占쎈쨨占쎈뙔占쎌굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅嶺뚮씭留뺧옙�맶�뜝�럥�쑅占쎌젂�뜝占� �뜝�럥�럸�뜝�럥利듸옙�쇀占쎈렰占쎌맶占쎈쐻�뜝占� �솾�꺂�뒧占쎈떔�뜝�룞�삕占쎈쐻占쎈윞�굜�깷�쐻占쎈윥占쎈젗占쎈쐻占쎈윥�걡�뼲�삕獄�占� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援�占쎌굲
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // �뜝�럥�맶�뜝�럥�쑋占쎈쐻占쎈쑕占쎌맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�꺐占썩벀�걠占쎄뎡 �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎈뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
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
	serverSock.sin_addr = netif->ip;

	net_proto_ip_print(serverSock.sin_addr);

	bind(ncsock,&serverSock,sizeof(sockaddr_in));

	char buffer[1024];
	int sfd = 0;
	sockaddr_in newsd;
	socklen_t len;

	Printf("svr fd id %d\n",ncsock);

	while(1)
	{
		sfd = accept(ncsock,&newsd,&len);
		Printf("accept fd id %d\n",sfd);
		break;
	}

	while(1)
	{
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
 // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
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
 *  �뜝�럥�맶�뜝�럥�쑋�뵓怨뺥�ワ옙�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑅�뜝�럥萸룟뜝�럩�뮋�뜝�럥猷긺뜏類���占쎌굲�뜝�럩�쓦�뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀧�뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑅嶺뚮쵓�삕
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮袁⑹��뜝�럡�꽟嶺뚳옙筌롪퉭鍮앭쉽占쏙옙�룂占쎄괴�뜝�럥�맶占쎈쐻�뜝占� 占쎈쐻占쎈윥占쎈룾�뜝�럡猿��뜝�럩�뮢�뜝�룞�삕
    InitializeParameter( &stList, pcParameterBuffer );
    // �뜝�럥�맶�뜝�럥�쑋占쎈쨨占쎈쾫占쎌맶�뜝�럥�쑅嶺뚮쵐�눓占쎌맶�뜝�럥�쑅�뜝�럥援� �솾�꺂�뒧占쎈뎔�뜝�럥琉끻뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈ご占쎄뭍占쎌맶�뜝�럥�쑋嶺뚮ㅏ諭븀빊占썲슖�볥걙占쎄뎡 �뜝�럥�맶�뜝�럥�쑅嶺뚯빓�뿥占쎌맶�뜝�럥吏쀥뜝�럩援꿰솾�꺂�뒧占쎈뎐占쎌뵗占쎈쐻占쎈윥�뤃占� 占쎈쐻占쎈윥占쎈룾占쎈쐻占쎈윥筌λ벩�쐻占쎈윪鈺곕쵓�삕占쎌맶�뜝�럥�쑅占쎈뎨�뜝�뜴�쐻占쎈쑆泳��굢�삕占쎄뎡 占쎈쐻占쎈윥�땻�쉩�쐻占쎈윞占쎄샴癲ル슣鍮�占쎌굲
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럡肄� �뜝�럥�꺐占썩뫀踰좑옙留놅옙�쐻占쎈윥占쎌몢 �뜝�럥�맶�뜝�럥�쑋占쎌븪�벉�맶�뜝�럥�쑋�뜝�럡猿� 占쎈쐻占쎈윥占쎈뼓癲ル슢�뀕占쎈쳮�뜝�럡猿묈뜝�럥�맶�뜝�럥�쑅�뜝�룞�삕�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뼨�곸뒅占쎌맶�뜝�럥�쑅占쎈쐻占쎈쑆占쎈솕�뜝�럩援뀐옙�쐻占쎈윥占쎄괵�뜝�럩�뮋�뜝�럥�솕�뜝�럩援� �솾�꺂�뒱占쎈쭓占쎈き占쎌뵛占쎌굲占쎄괌�뜝占�
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // �뜝�럥�맶�뜝�럥�쐾占쎈닱占쎌몱占쎌맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀�뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎌몱占쎌맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾占쎈쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援�占쎌굲
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

    // 占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥�굜占� �뜝�럩�뮋�뜝�럥猷긺뜏類ｋ꺏占쎌굲�뜝�럥彛у뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럩�뮋�뜝�럥猷긺뜏類ｋ걢�뜝�럩六쇘댆�뜉�삕占쎄뎡癲ル슢�뿪占쎌굲 0x7C05 �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럥援겼뜝�럥�맶�뜝�럥�쐾�뜝�럥彛� �뜝�럩�뀇�윜諭꾪맊占쎄튃占쎈쐻占쎈윞占쎈룺 �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄ゅ뜝�럥�걤嶺뚮　維쒙옙�굲�뜝�럥爾잌뜝�럩援� IA-32e �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄ゅ뜝�럥�걤嶺뚮ㅏ�뒩占쎄뎡占쎈쐻占쎈윥�뤃占�
    // �뜝�럥�맶�뜝�럥�쑅占쎈쐻占쎈쑕占쎌맶�뜝�럥�쑅占쎈뎨�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾬옙�쐻占쎈윥占쎄괴�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚횞�떋�띿삕�몭�씛�럷�뙴�옓�맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥筌ο옙 0x10000 �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럩�뮋�뜝�럥猷긺뜏類ｋ걢�뜝�럩六쇘댆�뜉�삕占쎄뎡�뜝�럥�렓占쎈눀�뙼�뿭�벝�뜝�럥�맶�뜝�럥�몶�뜝�럥�뱺�뜝�럥�맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥��耀붾굝梨멨뜝�룞�맀占쎌맶�뜝�럥吏쀥뜝�럩援뀐옙�쐻占쎈윥占쎄괵�뜝�럩�뮋�뜝�럥�솕�뜝�럩援�
    // 占쎈쐻占쎈윥占쎈뮉�뜝�럡肄ゅ뜝�럥�걤占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�럥猷뷴뜝�럥�맶�뜝�럥�쐾�뜝�럥�셾 �뜝�럥�맶�뜝�럥�쑅�뜝�럥臾꾤솾�꺂�뒧占쎈뎐�뜝�럥裕∽옙�눇�뙼蹂��굲 �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�솾�꺂�뒩占쎈뤈�뜝�럩援� 占쎈쐻占쎈쑆泳�占썲뜝�럡肄▼뜝�럡�뜦占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�벍�뜝�럥�맶�뜝�럥�쑅�뜝�럥彛у뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑋占쎄덩占쎈�섓옙�맶�뜝�럥�쑋�뵓怨ㅼ삕
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맳�뛾�룆梨룡뤃�댙�삕筌륅옙�뜝�럡�떅�뜝�럥�쓠占쎈쐻占쎈윞占쎄퐶�뜝�럥�맶�뜝�럥吏쀥뜝�럩援� �뜝�럥�맶�뜝�럥�쑋�뜝�럡�꼫�뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �솾�꺂�뒖占쎈１占쎌젂占쎈뼐占쎈��뜝�럡�꽫�뜝�럩援꿨뜝�럥鍮앾옙�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎌맶�뜝�럥�쑅�뜝�럥援� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援�占쎌굲
    //--------------------------------------------------------------------------
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾�뜝�럥夷у뜝�럥利멨뜝�럩援� �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑋�뜝�럡�꺘�뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅�뜏類㏃삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럡�맖�뜝�럥�맶�뜝�럥�쑅占쎈쨨占쎈섰占쎌맶�뜝�럥�쑋�뜝�럩�벂�뜝�럥�맶�뜝�럥�쑅�뜏類㏃삕
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�벍�뜝�럥�맶�뜝�럥�쑅�뜝�럥彛у뜝�럥�맶�뜝�럥�쑋�뵓怨ㅼ삕 �솾�꺂�뒱占쎈쭟�뜝�럩援� �뜝�럥�꺐占썩뫀踰좑옙留놅옙�쐻占쎈윥占쎌몢 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾
    pstItem = pstHeader->vstItem;

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅櫻뗫봿占싸살맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �솾�꺂�뒧占쎈�덃납占썲뜝�뜾�쎗�뜝�뜴�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �솾�꺂�뒖占쎈１占쎌젂占쎈뼐占쎈��뜝�럡�꽫�뜝�럩援꿨뜝�럥鍮앾옙�쐻�뜝占� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援�占쎌굲
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援꿨뜝�럥�맶�뜝�럥�쑅�뜝�럥援� �뜝�럥�맶�뜝�럥�쑅櫻뗫봿占싸살맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑋占쎌뼚伊쒙옙�굲占쎌뼔�▽펺�뜉�삕泳���彛쀯옙諭띰옙�뮋�뜝�럥�솕�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쐾占쎈닱筌롡뫀�맶�뜝�럥�쐾�뜝�럥�젾
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥苑▼뜝�럥�맶�뜝�럥�쐾�뜝�럥占쎌쉻�뵾占쎌뒩占쎈뤈�뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥�걛�뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쐾�뜝�럥�셾 占쎈쐻占쎈윞�뙼占썲뜝�럥�맶�뜝�럥�뱻�뇦猿곴뮈鴉딅뜉�삕泳�戮녹삕獄�占� �뜝�럥�맶�뜝�럥�쑅櫻뗫봿占싸살맶�뜝�럥�쑅嶺뚮씮�닲占쎌맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럥�맶�뜝�럥�쑋�뜝�럥痢쎾뜝�럥�맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎈뎨�뜝�룞�삕占쎌맶�뜝�럥�쑅占쎈쨨�뜝占� �뜝�럥�맶�뜝�럥�쑅�뜝�럡�떍�뜝�럥�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쐾占쎈뼀占쎌뒅占쎈뮋�뜝�럥�솕�뜝�럩援� �뜝�럩�뀇�윜諭꾪맊�떋�떟踰㏆옙援�占쎌굲
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂占쎌뿪占쎌굲�뜝�럥爾잌뜝�럩援� �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵐沅わ옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럩�뀇�윜諭꾪맔占쎌굲嚥싲갭�돘占쎌굲
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋�뛾占썲뜝占� �뜝�럥�맶�뜝�럥�쑅占쎌젂占쎌뵢占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕
        fl_fclose( fp );

        // �뜝�럥�맶�뜝�럥�쑅占쎌젂熬곻옙占쎌맶�뜝�럥�쑋�뵓怨ㅼ삕 �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌븮猿딆맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥吏쀥뜝�럩援꿨뜝�럥�맶�뜝�럥�쑋�뜝�럡�뀘�뜝�럥�맶�뜝�럥�쑅鶯ㅼ룊�삕 �뜝�럥�맶�뜝�럥�쑋嶺뚮씧�몚占쎈눚�뜝�럡��占쎈き占쎌몯�뜮�뀘�쐻�뜝占� �뜝�럥�맶�뜝�럥�쑋占쎌뼚짹占쎌맶�뜝�럥�쑅嶺뚯쉻�삕
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // �뜝�럥�맶�뜝�럥�쑅�뜝�럥爰듿뜝�럥�맶�뜝�럥�쑋占쎌뼲�삕 �뜝�럥�맶�뜝�럥�쑅嶺뚮쵐沅랃옙�맶�뜝�럥�쑅�뜏類ㅼ돥占쎌맶�뜝�럥�쑅占쎈쐻占쎈짗占쎌굲 �솾�꺂�뒱占쎌굲�뜝�럩�꼥占쎈쐻占쎈짗占쎌굲嶺뚮쵐沅わ옙�굲嶺뚮ㅏ�뼠占쎌맶占쎈쐻�뜝占� �뜝�럥�맶�뜝�럥�쐾�뜝�룞�삕�뜝�럩�뀇�윜諭꾪맔占쎌굲嚥싲갭�돘占쎌굲
   // FlushFileSystemCache();
}

