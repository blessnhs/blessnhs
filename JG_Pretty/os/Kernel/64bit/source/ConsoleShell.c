/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� �ҽ� ����
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
// Ŀ�ǵ� ���̺� ����
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
};

//==============================================================================
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
 */
void StartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    CONSOLEMANAGER* pstConsoleManager;
    
    // �ܼ��� �����ϴ� �ڷᱸ���� ��ȯ
    pstConsoleManager = GetConsoleManager();
    
    // ������Ʈ ���
    Printf( CONSOLESHELL_PROMPTMESSAGE );
    
    // �ܼ� �� ���� �÷��װ� TRUE�� �� ������ �ݺ�
    while( pstConsoleManager->bExit == FALSE )
    {

        bKey = GetCh();

        // �ܼ� �� ���� �÷��װ� ������ ��� ������ ����
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
                // ���� Ŀ�� ��ġ�� �� �� ���� ������ �̵��� ���� ������ ����ϰ� 
                // Ŀ�ǵ� ���ۿ��� ������ ���� ����
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
                // Ŀ�ǵ� ���ۿ� �ִ� ����� ����
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                ExecuteCommand( vcCommandBuffer );
            }
            
            // ������Ʈ ��� �� Ŀ�ǵ� ���� �ʱ�ȭ
            Printf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            MemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ����Ʈ Ű, CAPS Lock, NUM Lock, Scroll Lock�� ����
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
        else if( bKey < 128 )
        {
            // TAB�� �������� ��ȯ
            if( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            // ���۰� �������� ���� ����
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
 *  Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
 */
void ExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �������� ���е� Ŀ�ǵ带 ����
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ŀ�ǵ��� ���̿� ������ ������ ��ġ�ϴ��� �˻�
        if( ( iCommandLength == iSpaceIndex ) &&
            ( MemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ����Ʈ���� ã�� �� ���ٸ� ���� ���
    if( i >= iCount )
    {
        Printf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �Ķ���� �ڷᱸ���� �ʱ�ȭ
 */
void InitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������ ����� ���̸� ��ȯ
 */
int GetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ� ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �Ķ���͸� �����ϰ� ���̸� ��ȯ
    MemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������ ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���
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

    // ���� �� Ŀ�ǵ��� ���̸� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ���� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        Printf( "%s", gs_vstCommandTable[ i ].pcCommand );
        GetCursor( &iCursorX, &iCursorY );
        SetCursor( iMaxCommandLength, iCursorY );
        Printf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );

        // ����� ���� ��� ������ ������
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
 *  ȭ���� ���� 
 */
static void Cls( const char* pcParameterBuffer )
{

    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    ClearScreen();
    SetCursor( 0, 1 );
}

static void UsbReadSector( const char* pcParameterBuffer )
{
    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
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
	// �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
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
 *  �� �޸� ũ�⸦ ���
 */
static void ShowTotalRAMSize( const char* pcParameterBuffer )
{
    Printf( "Total RAM Size = %d MB\n", GetTotalRAMSize() );
}

/**
 *  PC�� �����(Reboot)
 */
static void Shutdown( const char* pcParamegerBuffer )
{
    Printf( "System Shutdown Start...\n" );
    
    // ���� �ý��� ĳ�ÿ� ����ִ� ������ �ϵ� ��ũ�� �ű�
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
    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����
    Printf( "Press Any Key To Reboot PC..." );
    GetCh();
    Reboot();
}

/**
 *  ���μ����� �ӵ��� ����
 */
static void MeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;
        
    Printf( "Now Measuring." );
    
    // 10�� ���� ��ȭ�� Ÿ�� ������ ī���͸� �̿��Ͽ� ���μ����� �ӵ��� ���������� ����
    DisableInterrupt();    
    for( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = ReadTSC();
        WaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += ReadTSC() - qwLastTSC;

        Printf( "." );
    }
    // Ÿ�̸� ����
    InitializePIT( MSTOCOUNT( 1 ), TRUE );
    EnableInterrupt();
    
    Printf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  RTC ��Ʈ�ѷ��� ����� ���� �� �ð� ������ ǥ��
 */
static void ShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // RTC ��Ʈ�ѷ����� �ð� �� ���ڸ� ����
    ReadRTCTime( &bHour, &bMinute, &bSecond );
    ReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    Printf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth,
             ConvertDayOfWeekToString( bDayOfWeek ) );
    Printf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

/**
 *  �½�ũ�� �켱 ������ ����
 */
static void ChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcPriority[ 30 ];
    QWORD qwID;
    BYTE bPriority;
    
    // �Ķ���͸� ����
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcPriority );
    
    // �½�ũ�� �켱 ������ ����
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
 *  ���� ������ ��� �½�ũ�� ������ ���
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
    
    // �ھ� ����ŭ ������ ���鼭 �� �����ٷ��� �ִ� �½�ũ�� ���� ���� 
    iProcessorCount = GetProcessorCount(); 
    
    for( i = 0 ; i < iProcessorCount ; i++ )
    {
        iTotalTaskCount += GetTaskCount( i );
    }
    
    Printf( "================= Task Total Count [%d] =================\n", 
             iTotalTaskCount );
    
    // �ھ 2�� �̻��̸� �� �����ٷ� ���� ������ ���
    if( iProcessorCount > 1 )
    {
        // �� �����ٷ� ���� �½�ũ�� ������ ���
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
            {
                Printf( "\n" );
            }
            
            SPrintf( vcBuffer, "Core %d : %d", i, GetTaskCount( i ) );
            Printf( vcBuffer );
            
            // ����ϰ� ���� ������ ��� �����̽��ٷ� ä��
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
        // TCB�� ���ؼ� TCB�� ��� ���̸� ID�� ���
        pstTCB = GetTCBInTCBPool( i );
        if( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            // �½�ũ�� 6�� ��µ� ������, ��� �½�ũ ������ ǥ������ ���θ� Ȯ��
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
 *  �½�ũ�� ����
 */
static void KillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    TCB* pstTCB;
    int i;
    
    // �Ķ���͸� ����
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    
    // �½�ũ�� ����
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // Ư�� ID�� �����ϴ� ���
    if( qwID != 0xFFFFFFFF )
    {
        pstTCB = GetTCBInTCBPool( GETTCBOFFSET( qwID ) );
        qwID = pstTCB->stLink.qwID;

        // �ý��� �׽�Ʈ�� ����
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
    // �ܼ� �а� ���� �½�ũ�� �����ϰ� ��� �½�ũ ����
    else
    {
        for( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        {
            pstTCB = GetTCBInTCBPool( i );
            qwID = pstTCB->stLink.qwID;

            // �ý��� �׽�Ʈ�� ���� ��Ͽ��� ����
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
 *  ���μ����� ������ ǥ��
 */
static void CPULoad( const char* pcParameterBuffer )
{
    int i;
    char vcBuffer[ 50 ];
    int iRemainLength;
    
    Printf( "================= Processor Load =================\n" ); 
    
    // �� �ھ� ���� ���ϸ� ���
    for( i = 0 ; i < GetProcessorCount() ; i++ )
    {
        if( ( i != 0 ) && ( ( i % 4 ) == 0 ) )
        {
            Printf( "\n" );
        }
        
        SPrintf( vcBuffer, "Core %d : %d%%", i, GetProcessorLoad( i ) );
        Printf( "%s", vcBuffer );
        
        // ����ϰ� ���� ������ ��� �����̽��ٷ� ä��
        iRemainLength = 19 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );
}

// ������ �߻���Ű�� ���� ����
static volatile QWORD gs_qwRandomValue = 0;

/**
 *  ������ ������ ��ȯ
 */
QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

/**
 *  ö�ڸ� �귯������ �ϴ� ������
 */
static void DropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        // ��� �����
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
 *  �����带 �����Ͽ� ��Ʈ���� ȭ��ó�� �����ִ� ���μ���
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

    // Ű�� �ԷµǸ� ���μ��� ����
    GetCh();
}

/**
 *  ��Ʈ���� ȭ���� ������
 */
static void ShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = CreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) MatrixProcess, TASK_LOADBALANCINGID );
    if( pstProcess != NULL )
    {
        Printf( "Matrix Process [0x%Q] Create Success\n" );

        // �½�ũ�� ���� �� ������ ���
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
 *  ���� �޸� ������ ǥ��
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
 *  �ϵ� ��ũ�� ������ ǥ��
 */
static void ShowHDDInformation( const char* pcParameterBuffer )
{
/*    HDDINFORMATION stHDD;
    char vcBuffer[ 100 ];
    
    // �ϵ� ��ũ�� ������ ����
    if( GetHDDInformation( &stHDD ) == FALSE )
    {
        Printf( "HDD Information Read Fail\n" );
        return ;
    }        
    
    Printf( "============ Primary Master HDD Information ============\n" );
    
    // �� ��ȣ ���
    MemCpy( vcBuffer, stHDD.vwModelNumber, sizeof( stHDD.vwModelNumber ) );
    vcBuffer[ sizeof( stHDD.vwModelNumber ) - 1 ] = '\0';
    Printf( "Model Number:\t %s\n", vcBuffer );
    
    // �ø��� ��ȣ ���
    MemCpy( vcBuffer, stHDD.vwSerialNumber, sizeof( stHDD.vwSerialNumber ) );
    vcBuffer[ sizeof( stHDD.vwSerialNumber ) - 1 ] = '\0';
    Printf( "Serial Number:\t %s\n", vcBuffer );

    // ���, �Ǹ���, �Ǹ��� �� ���� ���� ���
    Printf( "Head Count:\t %d\n", stHDD.wNumberOfHead );
    Printf( "Cylinder Count:\t %d\n", stHDD.wNumberOfCylinder );
    Printf( "Sector Count:\t %d\n", stHDD.wNumberOfSectorPerCylinder );
    
    // �� ���� �� ���
    Printf( "Total Sector:\t %d Sector, %dMB\n", stHDD.dwTotalSectors, 
            stHDD.dwTotalSectors / 2 / 1024 );*/
}

/**
 *  �ϵ� ��ũ�� �Ķ���ͷ� �Ѿ�� LBA ��巹������ ���� �� ��ŭ ����
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
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹���� ���� �� ����
    InitializeParameter( &stList, pcParameterBuffer );
    if( ( GetNextParameter( &stList, vcLBA ) == 0 ) ||
        ( GetNextParameter( &stList, vcSectorCount ) == 0 ) )
    {
        Printf( "ex) readsector 0(LBA) 10(count)\n" );
        return ;
    }
    dwLBA = AToI( vcLBA, 10 );
    iSectorCount = AToI( vcSectorCount, 10 );
    
    // ���� ����ŭ �޸𸮸� �Ҵ� �޾� �б� ����
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    if( ReadHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) == iSectorCount )
    {
        Printf( "LBA [%d], [%d] Sector Read Success~!!", dwLBA, iSectorCount );
        // ������ ������ ������ ���
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

                // ��� �� �ڸ��� ǥ���Ϸ��� 16���� ���� ��� 0�� �߰�����
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
 *  �ϵ� ��ũ�� �Ķ���ͷ� �Ѿ�� LBA ��巹������ ���� �� ��ŭ ��
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

    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� LBA ��巹���� ���� �� ����
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
    
    // ���۸� �Ҵ� �޾� �����͸� ä��. 
    // ������ 4 ����Ʈ�� LBA ��巹���� 4 ����Ʈ�� ���Ⱑ ����� Ƚ���� ����
    pcBuffer = AllocateMemory( iSectorCount * 512 );
    for( j = 0 ; j < iSectorCount ; j++ )
    {
        for( i = 0 ; i < 512 ; i += 8 )
        {
            *( DWORD* ) &( pcBuffer[ j * 512 + i ] ) = dwLBA + j;
            *( DWORD* ) &( pcBuffer[ j * 512 + i + 4 ] ) = s_dwWriteCount;            
        }
    }
    
    // ���� ����
    if( WriteHDDSector( TRUE, TRUE, dwLBA, iSectorCount, pcBuffer ) != iSectorCount )
    {
        Printf( "Write Fail\n" );
        return ;
    }
    Printf( "LBA [%d], [%d] Sector Write Success~!!", dwLBA, iSectorCount );

    // ������ ������ ������ ���
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

            // ��� �� �ڸ��� ǥ���Ϸ��� 16���� ���� ��� 0�� �߰�����
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
 *  �ϵ� ��ũ�� ����
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
 *  �ϵ� ��ũ�� ���� �ý����� ����(����)
 */
static void FormatHDD( const char* pcParameterBuffer )
{

    disk_t *disk = GetDisk(0);
	if(disk == 0)
	   	return;

	usb_msd_t* msd = disk->data;

	testMSD(msd);

	Printf("TotalSize %q \n",msd->disk.size/512);

	fl_format(msd->disk.size/512, "JG");

}

/**
 *  ���� �ý��� ������ ǥ��
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
 *  ��Ʈ ���͸��� �� ������ ����
 */
static void CreateFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_createdirectory(pcParameterBuffer);
}

/**
 *  ��Ʈ ���͸����� ������ ����
 */
static void DeleteFileInRootDirectory( const char* pcParameterBuffer )
{
	fl_remove(pcParameterBuffer);
}
/**
 *  ��Ʈ ���͸��� ���� ����� ǥ��
 */
static void ShowRootDirectory( const char* pcParameterBuffer )
{
	fl_listdirectory(pcParameterBuffer);
}

/**
 *  ������ �����Ͽ� Ű����� �Էµ� �����͸� ��
 */
static void WriteDataToFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // ���� ����
    fp = fl_fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // ���� Ű�� �������� 3�� ������ ������ ������ ���Ͽ� ��
    iEnterCount = 0;
    while( 1 )
    {
        bKey = GetCh();
        // ���� Ű�̸� ���� 3�� �������°� Ȯ���Ͽ� ������ ���� ����
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            if( iEnterCount >= 3 )
            {
                break;
            }
        }
        // ���� Ű�� �ƴ϶�� ���� Ű �Է� Ƚ���� �ʱ�ȭ
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
 *  ������ ��� �����͸� ����
 */
static void ReadDataFromFile( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 50 ];
    int iLength;
    FL_FILE* fp;
    int iEnterCount;
    BYTE bKey;
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
    InitializeParameter( &stList, pcParameterBuffer );
    iLength = GetNextParameter( &stList, vcFileName );
    vcFileName[ iLength ] = '\0';
    if( ( iLength > ( FILESYSTEM_MAXFILENAMELENGTH - 1 ) ) || ( iLength == 0 ) )
    {
        Printf( "Too Long or Too Short File Name\n" );
        return ;
    }
    
    // ���� ����
    fp = fl_fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // ������ ������ ����ϴ� ���� �ݺ�
    iEnterCount = 0;
    while( 1 )
    {
        if( fl_fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        Printf( "%c", bKey );
        
        // ���� ���� Ű�̸� ���� Ű Ƚ���� ������Ű�� 20���α��� ����ߴٸ� 
        // �� ������� ���θ� ���
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
 *  ���� �ý����� ĳ�� ���ۿ� �ִ� �����͸� ��� �ϵ� ��ũ�� �� 
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
 *  �ø��� ��Ʈ�κ��� �����͸� �����Ͽ� ���Ϸ� ����
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
    
    // �Ķ���� ����Ʈ�� �ʱ�ȭ�Ͽ� ���� �̸��� ����
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
    
    // �ø��� ��Ʈ�� FIFO�� ��� ���
    ClearSerialFIFO();
    
    //==========================================================================
    // ������ ���̰� ���ŵ� ������ ��ٸ��ٴ� �޽����� ����ϰ�, 4 ����Ʈ�� ������ ��
    // Ack�� ����
    //==========================================================================
    Printf( "Waiting For Data Length....." );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < 4 )
    {
        // ���� ����ŭ ������ ����
        dwTempSize = ReceiveSerialData( ( ( BYTE* ) &dwDataLength ) +
            dwReceivedSize, 4 - dwReceivedSize );
        dwReceivedSize += dwTempSize;
        
        // ���ŵ� �����Ͱ� ���ٸ� ��� ���
        if( dwTempSize == 0 )
        {
            Sleep( 0 );
            
            // ����� �ð��� 30�� �̻��̶�� Time Out���� ����
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 30000 )
            {
                Printf( "Time Out Occur~!!\n" );
                return ;
            }
        }
        else
        {
            // ���������� �����͸� ������ �ð��� ����
            qwLastReceivedTickCount = GetTickCount();
        }
    }
    Printf( "[%d] Byte\n", dwDataLength );

    // ���������� ������ ���̸� ���������Ƿ�, Ack�� �۽�
    SendSerialData( "A", 1 );

    //==========================================================================
    // ������ �����ϰ� �ø���κ��� �����͸� �����Ͽ� ���Ͽ� ����
    //==========================================================================
    // ���� ����
    fp = fopen( vcFileName, "w" );
    if( fp == NULL )
    {
        Printf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    
    // ������ ����
    Printf( "Data Receive Start: " );
    dwReceivedSize = 0;
    qwLastReceivedTickCount = GetTickCount();
    while( dwReceivedSize < dwDataLength )
    {
        // ���ۿ� ��Ƽ� �����͸� ��
        dwTempSize = ReceiveSerialData( vbDataBuffer, SERIAL_FIFOMAXSIZE );
        dwReceivedSize += dwTempSize;

        // �̹��� �����Ͱ� ���ŵ� ���� �ִٸ� ACK �Ǵ� ���� ���� ����
        if( dwTempSize != 0 ) 
        {
            // �����ϴ� ���� �������� ���������� �����߰ų� FIFO�� ũ���� 
            // 16 ����Ʈ���� �ѹ��� Ack�� ����
            if( ( ( dwReceivedSize % SERIAL_FIFOMAXSIZE ) == 0 ) ||
                ( ( dwReceivedSize == dwDataLength ) ) )
            {
                SendSerialData( "A", 1 );
                Printf( "#" );

            }
            
            // ���� �߿� ������ ����� �ٷ� ����
            if( fwrite( vbDataBuffer, 1, dwTempSize, fp ) != dwTempSize )
            {
                Printf( "File Write Error Occur\n" );
                break;
            }
            
            // ���������� �����͸� ������ �ð��� ����
            qwLastReceivedTickCount = GetTickCount();
        }
        // �̹��� ���ŵ� �����Ͱ� ���ٸ� ��� ���
        else
        {
            Sleep( 0 );
            
            // ����� �ð��� 10�� �̻��̶�� Time Out���� ����
            if( ( GetTickCount() - qwLastReceivedTickCount ) > 10000 )
            {
                Printf( "Time Out Occur~!!\n" );
                break;
            }
        }
    }   

    //==========================================================================
    // ��ü �������� ũ��� ������ ���� ���� �������� ũ�⸦ ���Ͽ� ���� ���θ�
    // ����� ��, ������ �ݰ� ���� �ý��� ĳ�ø� ��� ���
    //==========================================================================
    // ���ŵ� ���̸� ���ؼ� ������ �߻��ߴ����� ǥ��
    if( dwReceivedSize != dwDataLength )
    {
        Printf( "\nError Occur. Total Size [%d] Received Size [%d]\n", 
                 dwReceivedSize, dwDataLength );
    }
    else
    {
        Printf( "\nReceive Complete. Total Size [%d] Byte\n", dwReceivedSize );
    }
    
    // ������ �ݰ� ���� �ý��� ĳ�ø� ������
    fclose( fp );
    FlushFileSystemCache();*/
}

/**
 *  MP ���� ���̺� ������ ���
 */
static void ShowMPConfigurationTable( const char* pcParameterBuffer )
{
    PrintMPConfigurationTable();
}

/**
 *  IRQ�� I/O APIC�� ���ͷ�Ʈ �Է� ��(INTIN)�� ���踦 ������ ���̺��� ǥ��
 */
static void ShowIRQINTINMappingTable( const char* pcParameterBuffer )
{
    // I/O APIC�� �����ϴ� �ڷᱸ���� �ִ� ��� �Լ��� ȣ��
    PrintIRQToINTINMap();
}

/**
 *  �ھ� ���� ���ͷ�Ʈ�� ó���� Ƚ���� ���
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
    
    // MP ���� ���̺� ����� �ھ��� ������ ����
    iProcessCount = GetProcessorCount();
    
    //==========================================================================
    //  Column ���
    //==========================================================================
    // ���μ����� ����ŭ Column�� ���
    // �� �ٿ� �ھ� 4���� ����ϰ� �� Column�� 15ĭ�� �Ҵ���
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
        
        // ����ϰ� ���� ������ ��� �����̽��� ä��
        iRemainLength = 15 - kStrLen( vcBuffer );
        MemSet( vcBuffer, ' ', iRemainLength );
        vcBuffer[ iRemainLength ] = '\0';
        Printf( vcBuffer );
    }
    Printf( "\n" );

    //==========================================================================
    //  Row�� ���ͷ�Ʈ ó�� Ƚ�� ���
    //==========================================================================
    // �� ���ͷ�Ʈ Ƚ���� �ھ� �� ���ͷ�Ʈ ó�� Ƚ���� ���
    iLineCount = 0;
    pstInterruptManager = kGetInterruptManager();
    for( i = 0 ; i < INTERRUPT_MAXVECTORCOUNT ; i++ )
    {
        for( j = 0 ; j < iProcessCount ; j++ )
        {
            // Row�� ���, �� �ٿ� �ھ� 4���� ����ϰ� �� Column�� 15ĭ�� �Ҵ�
            if( j == 0 )
            {
                // 20 ���θ��� ȭ�� ����
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
            // ����ϰ� ���� ������ ��� �����̽��� ä��
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
 *  �½�ũ�� ���μ��� ģȭ���� ����
 */
static void ChangeTaskAffinity( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    char vcAffinity[ 30 ];
    QWORD qwID;
    BYTE bAffinity;
    
    // �Ķ���͸� ����
    InitializeParameter( &stList, pcParameterBuffer );
    GetNextParameter( &stList, vcID );
    GetNextParameter( &stList, vcAffinity );
    
    // �½�ũ ID �ʵ� ����
    if( MemCmp( vcID, "0x", 2 ) == 0 )
    {
        qwID = AToI( vcID + 2, 16 );
    }
    else
    {
        qwID = AToI( vcID, 10 );
    }
    
    // ���μ��� ģȭ��(Affinity) ����
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
 *  VBE ��� ���� ����� ���
 */
static void ShowVBEModeInfo( const char* pcParameterBuffer )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    
    // VBE ��� ���� ����� ��ȯ
    pstModeInfo = GetVBEModeInfoBlock();
    
    // �ػ󵵿� �� ������ ���ַ� ���
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
 *  �ý��� ���� �׽�Ʈ�ϴ� ���� ���� �½�ũ�� ����
 */
static void TestSystemCall( const char* pcParameterBuffer )
{
    BYTE* pbUserMemory;
    
    // ���� �Ҵ� ������ 4Kbyte �޸𸮸� �Ҵ� �޾� ���� ���� �½�ũ�� ������ �غ� ��
    pbUserMemory = AllocateMemory( 0x1000 );
    if( pbUserMemory == NULL )
    {
        return ;
    }
    
    // ���� ���� �½�ũ�� ����� SystemCallTestTask() �Լ��� �ڵ带 �Ҵ� ���� �޸𸮿� ����
    MemCpy( pbUserMemory, SystemCallTestTask, 0x1000 );
    
    // ���� ���� �½�ũ�� ����
    CreateTask( TASK_FLAGS_USERLEVEL | TASK_FLAGS_PROCESS,
            pbUserMemory, 0x1000, ( QWORD ) pbUserMemory, TASK_LOADBALANCINGID );
}

/**
 *  �������α׷��� ����
 */
static void ExecuteApplicationProgram( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcFileName[ 512 ];
    char vcArgumentString[ 1024 ];
    QWORD qwID;
    
    // �Ķ���͸� ����
    InitializeParameter( &stList, pcParameterBuffer );
    // ���Ŀ� ���� ������ ������ ����ϰ� ����
    if( GetNextParameter( &stList, vcFileName ) == 0 )
    {
        Printf( "ex)exec a.elf argument\n" );
        return ;
    }  

    // �� ��° ���� ���ڿ��� �ɼ����� ó��
    if( GetNextParameter( &stList, vcArgumentString ) == 0 )
    {
        vcArgumentString[ 0 ] = '\0';
    }
    
    Printf( "Execute Program... File [%s], Argument [%s]\n", vcFileName, 
            vcArgumentString );
    
    // �½�ũ ����
    qwID = ExecuteProgram( vcFileName, vcArgumentString, TASK_LOADBALANCINGID );
    Printf( "Task ID = 0x%Q\n", qwID );
}

/**
 *  ��Ű���� ����ִ� �����͸� �ϵ� ��ũ�� ����
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

    // ��Ʈ �δ��� �ε��� 0x7C05 ��巹������ ��ȣ ��� Ŀ�ΰ� IA-32e ��� Ŀ����
    // ���� ���� ���� ����
    wKernelTotalSectorCount = *( ( WORD* ) 0x7C05 );

    // ��ũ �̹����� 0x10000 ��巹���� �ε��ǹǷ� �̸� ��������
    // Ŀ�� ���� ����ŭ ������ ���� ��Ű�� ����� ����
    pstHeader = ( PACKAGEHEADER* ) ( ( QWORD ) 0x10000 + wKernelTotalSectorCount * 512 );

    // �ñ׳�ó�� Ȯ��
    if( MemCmp( pstHeader->vcSignature, PACKAGESIGNATURE,
                 sizeof( pstHeader->vcSignature ) ) != 0 )
    {
        Printf( "Package Signature Fail\n" );
        return ;
    }

    //--------------------------------------------------------------------------
    // ��Ű�� ���� ��� ������ ã�Ƽ� �ϵ� ��ũ�� ����
    //--------------------------------------------------------------------------
    // ��Ű�� �����Ͱ� �����ϴ� ��巹��
    qwDataAddress = ( QWORD ) pstHeader + pstHeader->dwHeaderSize;
    // ��Ű�� ����� ù ��° ���� ������
    pstItem = pstHeader->vstItem;

    // ��Ű���� ���Ե� ��� ������ ã�Ƽ� ����
    for( i = 0 ; i < pstHeader->dwHeaderSize / sizeof( PACKAGEITEM ) ; i++ )
    {
        Printf( "[%d] file: %s, size: %d Byte\n", i + 1, pstItem[ i ].vcFileName,
                 pstItem[ i ].dwFileLength );

        // ��Ű���� ���Ե� ���� �̸����� ������ ����
        fp = fl_fopen( pstItem[ i ].vcFileName, "w" );
        if( fp == NULL )
        {
            Printf( "%s File Create Fail\n" );
            return ;
        }

        // ��Ű�� ������ �κп� ���Ե� ���� ������ �ϵ� ��ũ�� ����
        if( fl_fwrite( ( BYTE* ) qwDataAddress, 1, pstItem[ i ].dwFileLength, fp ) !=
                pstItem[ i ].dwFileLength )
        {
            Printf( "Write Fail\n" );

            // ������ �ݰ� ���� �ý��� ĳ�ø� ������
            fl_fclose( fp );
           // FlushFileSystemCache();

            return ;
        }

        // ������ ����        
        fl_fclose( fp );

        // ���� ������ ����� ��ġ�� �̵�
        qwDataAddress += pstItem[ i ].dwFileLength;
    }

    Printf( "Package Install Complete\n" );

    // ���� �ý��� ĳ�ø� ������
   // FlushFileSystemCache();
}

