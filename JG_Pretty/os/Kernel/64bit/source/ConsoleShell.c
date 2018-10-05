/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   콘솔 셸에 관련된 소스 파일
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
#include "NETWORK/network.h"
#include "NETPROTOCOL/tcp.h"
#include "NETPROTOCOL/dns_help.h"
#include "utility.h"
#include "fat32/fat_defs.h"
#include "fat32/fat_filelib.h"
// 커맨드 테이블 정의
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
};

//==============================================================================
//  실제 셸을 구성하는 코드
//==============================================================================
/**
 *  셸의 메인 루프
 */
void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // 콘솔을 관리하는 자료구조를 반환
    pstConsoleManager = GetConsoleManager();
    
    // 프롬프트 출력
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // 콘솔 셸 종료 플래그가 TRUE가 될 때까지 반복
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // 콘솔 셸 종료 플래그가 설정된 경우 루프를 종료
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
                // 현재 커서 위치를 얻어서 한 문자 앞으로 이동한 다음 공백을 출력하고 
                // 커맨드 버퍼에서 마지막 문자 삭제
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
                // 커맨드 버퍼에 있는 명령을 실행
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // 프롬프트 출력 및 커맨드 버퍼 초기화
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // 시프트 키, CAPS Lock, NUM Lock, Scroll Lock은 무시
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB은 공백으로 전환
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // 버퍼가 남아있을 때만 가능
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
 *  커맨드 버퍼에 있는 커맨드를 비교하여 해당 커맨드를 처리하는 함수를 수행
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // 공백으로 구분된 커맨드를 추출
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // 커맨드 테이블을 검사해서 동일한 이름의 커맨드가 있는지 확인
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // 커맨드의 길이와 내용이 완전히 일치하는지 검사
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // 리스트에서 찾을 수 없다면 에러 출력
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  파라미터 자료구조를 초기화
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  공백으로 구분된 파라미터의 내용과 길이를 반환
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // 더 이상 파라미터가 없으면 나감
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // 버퍼의 길이만큼 이동하면서 공백을 검색
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // 파라미터를 복사하고 길이를 반환
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // 파라미터의 위치 업데이트
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  커맨드를 처리하는 코드
//==============================================================================
/**
 *  셸 도움말을 출력
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

    // 가장 긴 커맨드의 길이를 계산
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // 도움말 출력
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // 목록이 많을 경우 나눠서 보여줌
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
 *  화면을 지움 
 */
static void Cls( const char* pcParameterBuffer )
{

    // 맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후, 라인 1로 커서 이동
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // 맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후, 라인 1로 커서 이동
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
	  char buffer[512];
	  size_t waitingDataCommand = 0, renaming = 0, enterPasvMode = 0, fileTransfer = 0, binaryFileTransfer = 0, saveFile = 0, storeFile = 0, fileSize = 0, maxFileSize = 30000;

	  bool readykeyinput = 0;
	  uint8_t fileData[maxFileSize];
	  char command[200], hostname[100], user[100], pass[100], ctrlPort[10];

	  IP4_t IP = getAddrByName("ftp.sunet.se");
	  IP4_t dataIP;

	  Printf("%d.%d.%d.%d\n",IP.IP4[0],IP.IP4[1],IP.IP4[2],IP.IP4[3]);

	  uint16_t controlPort = 21,dataPort;
	  uint32_t control = tcp_uconnect(IP, controlPort),dataConnection = 0;;

	  Printf("\nConnected (ID = %u). Wait until connection is established...\n", control);
	  EVENT_t ev = EVENT_NONE;

	  memcpy(user,"anonymous",50);

	  memset(command,0,200);
	  int commandindex = 0;

	  while(1)
	  {
		  //KEY_t* key = (void*)buffer;
		  //EVENT_KEY_DOWN
		  if(readykeyinput == 1)
		  {
			  char ch = GetCh();
			  Printf("%c",ch);
			  buffer[commandindex++] = ch;
			  if(ch == '\n')
			  {
				  Printf("------");
				  buffer[commandindex++] = 0;
				  ev = EVENT_KEY_DOWN;
				  commandindex = 0;
			  }
		  }

		  switch (ev)
		  {
	              case EVENT_TCP_CONNECTED:
	              {
	                  tcpConnectedEventHeader_t* header = (void*)buffer;
	                  if (header->connectionID == dataConnection && waitingDataCommand)
	                  {
	  			         waitingDataCommand = 0;
	                     tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  break;
	              }
	              case EVENT_TCP_CLOSED:
	              {
	                  uint32_t connectionID = *(uint32_t*)buffer;
	                  if (connectionID == dataConnection)
	                  {
	                      Printf("Closed dataConnection.\n");
	                  }
	                  break;
	              }
	              case EVENT_TCP_RECEIVED:
	              {
	                  tcpReceivedEventHeader_t* header = (void*)buffer;

	                  if (header->connectionID == dataConnection)
	                  {
	                      if (fileTransfer)
	                      {
	                          if (binaryFileTransfer)
	                          {
	                              uint8_t* data = (void*)(header+1);
	                              data[header->length] = 0;
	                              memcpy(fileData+fileSize, data, header->length);
	                              fileSize += header->length;
	                          }
	                          else
	                          {
	                              char* data = (void*)(header+1);
	                              data[header->length] = 0;
	                              memcpy(fileData+fileSize, data, header->length);
	                              fileSize += header->length;
	                          }
	                      }
	                      else
	                      {
	                          char* data = (void*)(header+1);
	                          data[header->length] = 0;

	                          Printf("%s\n", data);
	                      }
	                  }
	                  else if (header->connectionID == control)
	                  {
	                      char* data = (void*)(header+1);
	                      data[header->length] = 0;
	                      Printf("%s\n", data);

	                      if (data[0] == '1' && data[1] == '5' && data[2] == '0')
	                      {
	                          if (storeFile)
	                          {
	                              storeFile = 0;
	                              Printf("Enter filename/path to load(e.g.:\"1:/test.txt\",max. 15 characters):\n");
	                              char filename[15];
	                              gets_s(filename, 15);
	                              FL_FILE* f = fl_fopen(filename, "r");

	                              char c;
	                              while (1)
	                              {
	                                  c = 0;//fgetc(f);
	                                  if(c == -1)
	                                      break;
	                                  memcpy(fileData+fileSize, &c, 1);
	                                  fileSize++;
	                              }

	                              size_t mod = (fileSize % 1460), packets = ((fileSize - mod) / 1460);
	                              size_t i = 0;
	                              while (i < packets)
	                              {
	                            	  tcp_send(tcp_findConnectionID(dataConnection), fileData+i*1460, 1460);
	                                  i++;
	                              }
	                              if (mod > 0)
	                              {
	                            	  tcp_send(tcp_findConnectionID(dataConnection), fileData+i*1460, mod);
	                              }
	                              tcp_close(dataConnection);
	                              fl_fclose(f);
	                          }
	                      }
	                      else if (data[0] == '2' && data[1] == '0' && data[2] == '0')
	                      {
	                          if (enterPasvMode)
	                          {
	                              enterPasvMode = 0;
	                              tcp_send(tcp_findConnectionID(control), "PASV\r\n", 6);
	                          }
	                      }
	                      else if (data[0] == '2' && data[1] == '2' && data[2] == '0')
	                      {
	                          char pStr[200];
	                          SPrintf(pStr,  "USER %s\r\n", user);
	                          tcp_send(tcp_findConnectionID(control), pStr, strlen(pStr));
	                      }
	                      else if (data[0] == '2' && data[1] == '2' && data[2] == '6')
	                      {
	                          if (saveFile)
	                          {
	                              saveFile = 0;
	                              fileTransfer = 0;
	                              Printf("Save file.\n\nEnter filename/path(e.g.:\"1:/test.txt\",max. 15 characters):\n");
	                              char saveFileName[15];
	                              gets_s(saveFileName, 15);
	                              FL_FILE* f = fl_fopen(saveFileName, "w");
	                              if (binaryFileTransfer)
	                              {
	                                  if (fl_fwrite(fileData, fileSize, 1, f))
	                                  {
	                                      Printf("\n%s:\nOK.\n\n", saveFileName);
	                                  }
	                              }
	                              else
	                              {
	                                  if (fl_fwrite(fileData, fileSize, 1, f))
	                                  {
	                                      Printf("\n%s:\nOK.\n", saveFileName);
	                                  }
	                              }
	                              fl_fclose(f);
	                          }
	                          tcp_close(tcp_findConnectionID(dataConnection));
	                      }
	                      else if (data[0] == '2' && data[1] == '2' && data[2] == '7')
	                      {
	                          uint8_t temp[6];
	                          uint8_t it = 3;
	                          do
	                          {
	                              if (data[it] == '(')
	                                  break;
	                          } while (it++);
	                          uint8_t i_start,i_end,byte;
	                          for (i_start = it+1, i_end = it+1, byte = 0; byte < 6; i_end++)
	                          {
	                              if (data[i_end] == ')')
	                              {
	                                  temp[byte] = atoi(data+i_start);
	                                  break;
	                              }

	                              if (data[i_end] == ',')
	                              {
	                                  data[i_end] = 0;
	                                  temp[byte] = atoi(data+i_start);
	                                  i_start = i_end+1;
	                                  byte++;
	                              }
	                          }

	                          int i;
	                          for (i = 0;i < 4;i++)
	                              dataIP.IP4[i] = temp[i];
	                          dataPort = temp[4]*256+temp[5];

	                          dataConnection = tcp_uconnect(dataIP, dataPort);
	                          Printf("Connected (ID = %u)...\n ", dataConnection);
	                      }
	                      else if (data[0] == '2' && data[1] == '3' && data[2] == '0')
	                      {
	                          Printf("Loggin successful.\n\n");
	                          tcp_send(tcp_findConnectionID(control), "OPTS UTF8 ON\r\n", 14);

	                          readykeyinput = 1;
	                      }
	                      else if (data[0] == '3' && data[1] == '3' && data[2] == '1')
	                      {
	                          char pStr[200];
	                          SPrintf(pStr, "PASS %s\r\n", pass);
	                          tcp_send(tcp_findConnectionID(control), pStr, strlen(pStr));
	                      }
	                      else if (data[0] == '3' && data[1] == '5' && data[2] == '0')
	                      {
	                          if (renaming)
	                          {
	                              renaming = 0;
	                              tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                          }
	                      }
	                      else if (data[0] == '5' && data[1] == '5' && data[2] == '0')
	                      {
	                          if (fileTransfer)
	                          {
	                              saveFile = 0;
	                              fileTransfer = 0;
	                          }
	                      }
	                      /*
	                      FTP reply codes (http://www.w3.org/Protocols/rfc959/9_References.html):
	                          150 File status okay; about to open data connection.
	                          200 Command okay.
	                          220 Service ready for new user.
	                          226 Closing data connection. Requested file action successful (for example, file transfer or file abort).
	                          227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
	                          230 User logged in, proceed.
	                          331 User name okay, need password.
	                          350 Requested file action pending further information.
	                          550 Requested action not taken. File unavailable (e.g., file not found, no access).
	                      */
	                  }
	                  break;
	              }
	              case EVENT_KEY_DOWN:
	              {
	                  KEY_t* key = (void*)buffer;
	                  if (*key == KEY_ESC)
	                  {
	                      tcp_send(tcp_findConnectionID(control), "QUIT\r\n", 6);
	                      tcp_close(tcp_findConnectionID(control));
	                      //return (0);
	                      break;
	                  }
	                  else if (*key == KEY_F1)
	                  {
	                      waitingDataCommand = 1;
	                      enterPasvMode = 1;
	                      Printf("Get file(ASCII-mode).\nEnter filename:\n");
	                      char filename[100];
	                      gets_s(filename, 100);
	                      memset(command,0,200);
	                      strcat(command,"RETR ");
	                      strcat(command,filename);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), "TYPE A\r\n", 8);
	                  }
	                  else if (*key == KEY_F2)
	                  {
	                      waitingDataCommand = 1;
	                      enterPasvMode = 1;
	                      saveFile = 1;
	                      fileTransfer = 1;
	                      binaryFileTransfer = 0;
	                      fileSize = 0;
	                      Printf("Save file.(max. size: %u)\nEnter filename(server):\n", maxFileSize);
	                      char filename[100];
	                      gets_s(filename, 100);
	                      Printf("\nEnter transfer mode(A == ASCII, I == Binary):\n");
	                      char mode[1];
	                      gets_s(mode, 1);
	                      memset(command,0,200);
	                      strcat(command,"RETR ");
	                      strcat(command,filename);
	                      strcat(command,"\r\n");
	                      memset(fileData, 0, 20000);
	                      Printf('\n');
	                      if (mode[0] == 'I')
	                      {
	                          binaryFileTransfer = 1;
	                          tcp_send(tcp_findConnectionID(control), "TYPE I\r\n", 8);
	                      }
	                      else
	                    	  tcp_send(tcp_findConnectionID(control), "TYPE A\r\n", 8);
	                  }
	                  else if (*key == KEY_F3)
	                  {
	                      waitingDataCommand = 1;
	                      enterPasvMode = 1;
	                      storeFile = 1;
	                      fileSize = 0;
	                      Printf("Store file(ASCII-mode).\n\nEnter filename(server):\n");
	                      char filename[100];
	                      gets_s(filename, 100);
	                      memset(command,0,200);
	                      strcat(command,"STOR ");
	                      strcat(command,filename);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), "TYPE A\r\n", 8);
	                  }
	                  else if (*key == KEY_F4)
	                  {
	                      //Delete file
	                      Printf("Delete file.\nEnter filename:\n");
	                      char filename[100];
	                      gets_s(filename, 100);
	                      memset(command,0,200);
	                      strcat(command,"DELE ");
	                      strcat(command,filename);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  else if (*key == KEY_F5)
	                  {
	                      //Create directory
	                      Printf("Create directory.\nEnter directory name:\n");
	                      char dirname[100];
	                      gets_s(dirname, 100);
	                      memset(command,0,200);
	                      strcat(command,"MKD ");
	                      strcat(command,dirname);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  else if (*key == KEY_F6)
	                  {
	                      //Remove directory
	                      Printf("Remove directory.\nEnter directory name:\n");
	                      char filename[100];
	                      gets_s(filename, 100);
	                      memset(command,0,200);
	                      strcat(command,"RMD ");
	                      strcat(command,filename);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  else if (*key == KEY_F7)
	                  {
	                      //Change current directory
	                      Printf("Change current directory.\n\nEnter directory name(without initiating / !):\n");
	                      char dirname[100];
	                      gets_s(dirname, 100);
	                      memset(command,0,200);
	                      strcat(command,"CWD ");
	                      strcat(command,dirname);
	                      strcat(command,"\r\n");
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  else if (*key == KEY_F8)
	                  {
	                      //Get current directory
	                      Printf("Current directory is:\n\n");
	                      tcp_send(control, "PWD\r\n", 5);
	                  }
	                  else if (*key == KEY_F9)
	                  {
	                      //List files/directories
	                      waitingDataCommand = 1;
	                      enterPasvMode = 1;
	                      memset(command,0,200);
	                      strcat(command,"LIST -a\r\n");
	                      Printf("List files/directories.\n\n");
	                      tcp_send(tcp_findConnectionID(control), "TYPE A\r\n", 8);
	                  }
	                  else if (*key == KEY_F10)
	                  {
	                      //Rename
	                      renaming = 1;
	                      Printf("Rename.\nEnter current filename:\n");
	                      char oldFilename[100];
	                      gets_s(oldFilename, 100);
	                      Printf("\nEnter new filename:\n");
	                      char newFilename[100];
	                      gets_s(newFilename, 100);
	                      memset(command,0,200);
	                      strcat(command,"RNTO ");
	                      strcat(command,newFilename);
	                      strcat(command,"\r\n");
	                      char tempCommand[200];
	                      SPrintf(tempCommand, "RNFR %s\r\n", oldFilename);
	                      Printf('\n');
	                      tcp_send(tcp_findConnectionID(control), tempCommand, strlen(tempCommand));
	                  }
	                  else if (*key == KEY_F11)
	                  {
	                      Printf("Change file/directory permissions.\n\nEnter file/directory:\n");
	                      char filename[183];
	                      gets_s(filename, 183);
	                      Printf("\nEnter new file/directory permissions (e.g. 644):\n");
	                      char permissions[3];
	                      gets_s(permissions, 3);
	                      Printf('\n');
	                      memset(command,0,200);
	                      SPrintf(command, "SITE CHMOD %s %s\r\n", permissions, filename);
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  else if (*key == KEY_F12)
	                  {
	                      Printf("Enter command:\n");
	                      memset(command,0,200);
	                      gets_s(command, 200);
	                      strcat(command,"\r\n");
	                      tcp_send(tcp_findConnectionID(control), command, strlen(command));
	                  }
	                  break;
	              }
	              default:
	                  break;
	          }

	         ev = event_poll(buffer, 4096, EVENT_NONE,TCP_T,21,control);

	  		if (0 != ev)
	  		{
	  			Printf("event %d\n", ev);
	  		}
	  }

	//  		tcp_close(control);
}


static void UsbWriteSector( const char* pcParameterBuffer )
{
	// 맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후, 라인 1로 커서 이동
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
 *  총 메모리 크기를 출력
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC를 재시작(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // 파일 시스템 캐시에 들어있는 내용을 하드 디스크로 옮김
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
    // 키보드 컨트롤러를 통해 PC를 재시작
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  프로세서의 속도를 측정
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10초 동안 변화한 타임 스탬프 카운터를 이용하여 프로세서의 속도를 간접적으로 측정
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // 타이머 복원
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC 컨트롤러에 저장된 일자 및 시간 정보를 표시
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC 컨트롤러에서 시간 및 일자를 읽음
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  태스크의 우선 순위를 변경
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // 파라미터를 추출
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // 태스크의 우선 순위를 변경
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
 *  현재 생성된 모든 태스크의 정보를 출력
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
    
    // 코어 수만큼 루프를 돌면서 각 스케줄러에 있는 태스크의 수를 더함 
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // 코어가 2개 이상이면 각 스케줄러 별로 개수를 출력
    if( iProcessorCount > 1 )
    {
        // 각 스케줄러 별로 태스크의 개수를 출력
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // 출력하고 남은 공간을 모두 스페이스바로 채움
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
        // TCB를 구해서 TCB가 사용 중이면 ID를 출력
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // 태스크가 6개 출력될 때마다, 계속 태스크 정보를 표시할지 여부를 확인
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
 *  태스크를 종료
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // 파라미터를 추출
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // 태스크를 종료
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 특정 ID만 종료하는 경우
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // 시스템 테스트는 제외
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
    // 콘솔 셸과 유휴 태스크를 제외하고 모든 태스크 종료
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // 시스템 테스트는 삭제 목록에서 제외
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
 *  프로세서의 사용률을 표시
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // 각 코어 별로 부하를 출력
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // 출력하고 남은 공간을 모두 스페이스바로 채움
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// 난수를 발생시키기 위한 변수
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  임의의 난수를 반환
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  철자를 흘러내리게 하는 스레드
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // 잠시 대기함
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
 *  스레드를 생성하여 매트릭스 화면처럼 보여주는 프로세스
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

    // 키가 입력되면 프로세스 종료
    GetCh();
}

/**
 *  매트릭스 화면을 보여줌
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // 태스크가 종료 될 때까지 대기
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
 *  동적 메모리 정보를 표시
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
 *  하드 디스크의 정보를 표시
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // 하드 디스크의 정보를 읽음
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // 모델 번호 출력
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // 시리얼 번호 출력
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // 헤드, 실린더, 실린더 당 섹터 수를 출력
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // 총 섹터 수 출력
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  하드 디스크에 파라미터로 넘어온 LBA 어드레스에서 섹터 수 만큼 읽음
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
    
    // 파라미터 리스트를 초기화하여 LBA 어드레스와 섹터 수 추출
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // 섹터 수만큼 메모리를 할당 받아 읽기 수행
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // 데이터 버퍼의 내용을 출력
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

                // 모두 두 자리로 표시하려고 16보다 작은 경우 0을 추가해줌
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
 *  하드 디스크에 파라미터로 넘어온 LBA 어드레스에서 섹터 수 만큼 씀
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

    // 파라미터 리스트를 초기화하여 LBA 어드레스와 섹터 수 추출
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
    
    // 버퍼를 할당 받아 데이터를 채움. 
    // 패턴은 4 바이트의 LBA 어드레스와 4 바이트의 쓰기가 수행된 횟수로 생성
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // 쓰기 수행
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // 데이터 버퍼의 내용을 출력
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

            // 모두 두 자리로 표시하려고 16보다 작은 경우 0을 추가해줌
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
 *  하드 디스크를 연결
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
 *  하드 디스크에 파일 시스템을 생성(포맷)
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
 *  파일 시스템 정보를 표시
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
 *  루트 디렉터리에 빈 파일을 생성
 */
static void CreateFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_createdirectory(pcParameterBuffer);
}

static void CreateDirectory( const char* pcParameterBuffer )
{
	char name[256];
	PARAMETERLIST stList;

	// 파라미터를 추출
	InitializeParameter( &stList, pcParameterBuffer );
	GetNextParameter( &stList, name );

	Printf( "mkdir %s\n",name);

	fl_createdirectory(name);
}

/**
 *  루트 디렉터리에서 파일을 삭제
 */
static void DeleteFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  루트 디렉터리의 파일 목록을 표시
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  파일을 생성하여 키보드로 입력된 데이터를 씀
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 파라미터 리스트를 초기화하여 파일 이름을 추출
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 파일 생성
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 엔터 키가 연속으로 3번 눌러질 때까지 내용을 파일에 씀
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // 엔터 키이면 연속 3번 눌러졌는가 확인하여 루프를 빠져 나감
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // 엔터 키가 아니라면 엔터 키 입력 횟수를 초기화
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
 *  파일을 열어서 데이터를 읽음
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // 파라미터 리스트를 초기화하여 파일 이름을 추출
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // 파일 생성
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 파일의 끝까지 출력하는 것을 반복
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // 만약 엔터 키이면 엔터 키 횟수를 증가시키고 20라인까지 출력했다면 
        // 더 출력할지 여부를 물어봄
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
 *  파일 시스템의 캐시 버퍼에 있는 데이터를 모두 하드 디스크에 씀 
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
 *  시리얼 포트로부터 데이터를 수신하여 파일로 저장
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
    
    // 파라미터 리스트를 초기화하여 파일 이름을 추출
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
    
    // 시리얼 포트의 FIFO를 모두 비움
    ClearSerialFIFO();
    
    //==========================================================================
    // 데이터 길이가 수신될 때까지 기다린다는 메시지를 출력하고, 4 바이트를 수신한 뒤
    // Ack를 전송
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // 남은 수만큼 데이터 수신
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // 수신된 데이터가 없다면 잠시 대기
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // 대기한 시간이 30초 이상이라면 Time Out으로 중지
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // 마지막으로 데이터를 수신한 시간을 갱신
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // 정상적으로 데이터 길이를 수신했으므로, Ack를 송신
    SendSerialData( "A", 1 );

    //==========================================================================
    // 파일을 생성하고 시리얼로부터 데이터를 수신하여 파일에 저장
    //==========================================================================
    // 파일 생성
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // 데이터 수신
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // 버퍼에 담아서 데이터를 씀
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // 이번에 데이터가 수신된 것이 있다면 ACK 또는 파일 쓰기 수행
        if( dwTempSize != 0 ) 
        {
            // 수신하는 쪽은 데이터의 마지막까지 수신했거나 FIFO의 크기인 
            // 16 바이트마다 한번씩 Ack를 전송
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // 쓰기 중에 문제가 생기면 바로 종료
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // 마지막으로 데이터를 수신한 시간을 갱신
            qwLastReceivedTickCount = GetTickCount();
        }
        // 이번에 수신된 데이터가 없다면 잠시 대기
        else
        {
            Sleep( 0 );
            
            // 대기한 시간이 10초 이상이라면 Time Out으로 중지
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // 전체 데이터의 크기와 실제로 수신 받은 데이터의 크기를 비교하여 성공 여부를
    // 출력한 뒤, 파일을 닫고 파일 시스템 캐시를 모두 비움
    //==========================================================================
    // 수신된 길이를 비교해서 문제가 발생했는지를 표시
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // 파일을 닫고 파일 시스템 캐시를 내보냄
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP 설정 테이블 정보를 출력
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ와 I/O APIC의 인터럽트 입력 핀(INTIN)의 관계를 저장한 테이블을 표시
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC를 관리하는 자료구조에 있는 출력 함수를 호출
    PrintIRQToINTINMap();
}

/**
 *  코어 별로 인터럽트를 처리한 횟수를 출력
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
    
    // MP 설정 테이블에 저장된 코어의 개수를 읽음
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column 출력
    //==========================================================================
    // 프로세서의 수만큼 Column을 출력
    // 한 줄에 코어 4개씩 출력하고 한 Column당 15칸을 할당함
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
        
        // 출력하고 남은 공간을 모두 스페이스로 채움
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row와 인터럽트 처리 횟수 출력
    //==========================================================================
    // 총 인터럽트 횟수와 코어 별 인터럽트 처리 횟수를 출력
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row를 출력, 한 줄에 코어 4개씩 출력하고 한 Column당 15칸을 할당
            if( j == 0 )
            {
                // 20 라인마다 화면 정지
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
            // 출력하고 남은 영역을 모두 스페이스로 채움
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
 *  태스크의 프로세서 친화도를 변경
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // 파라미터를 추출
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // 태스크 ID 필드 추출
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // 프로세서 친화도(Affinity) 추출
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
 *  VBE 모드 정보 블록을 출력
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE 모드 정보 블록을 반환
    pstModeInfo = GetVBEModeInfoBlock();
    
    // 해상도와 색 정보를 위주로 출력
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
 *  시스템 콜을 테스트하는 유저 레벨 태스크를 생성
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // 동적 할당 영역에 4Kbyte 메모리를 할당 받아 유저 레벨 태스크를 생성할 준비를 함
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // 유저 레벨 태스크로 사용할 SystemCallTestTask() 함수의 코드를 할당 받은 메모리에 복사
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // 유저 레벨 태스크로 생성
    CreateTask( TASK_FLAGS_USERLEVEL | TASK_FLAGS_PROCESS,
            pbUserMemory, 0x1000, ( QWORD ) pbUserMemory, TASK_LOADBALANCINGID );
}

/**
 *  응용프로그램을 실행
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // 파라미터를 추출
    InitializeParameter( &stList, pcParameterBuffer );
    // 형식에 맞지 않으면 도움말을 출력하고 종료
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // 두 번째 인자 문자열은 옵션으로 처리
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // 태스크 생성
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  패키지에 들어있는 데이터를 하드 디스크에 복사
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

    // 부트 로더가 로딩된 0x7C05 어드레스에서 보호 모드 커널과 IA-32e 모드 커널을
    // 합한 섹터 수를 읽음
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // 디스크 이미지는 0x10000 어드레스에 로딩되므로 이를 기준으로
    // 커널 섹터 수만큼 떨어진 곳에 패키지 헤더가 있음
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // 시그너처를 확인
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // 패키지 내의 모든 파일을 찾아서 하드 디스크에 복사
    //--------------------------------------------------------------------------
    // 패키지 데이터가 시작하는 어드레스
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // 패키지 헤더의 첫 번째 파일 데이터
    pstItem = pstHeader->vstItem;

    // 패키지에 포함된 모든 파일을 찾아서 복사
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // 패키지에 포함된 파일 이름으로 파일을 생성
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // 패키지 데이터 부분에 포함된 파일 내용을 하드 디스크로 복사
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // 파일을 닫고 파일 시스템 캐시를 내보냄
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // 파일을 닫음        
        fl_fclose( fp );

        // 다음 파일이 저장된 위치로 이동
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // 파일 시스템 캐시를 내보냄
   // FlushFileSystemCache();
}

