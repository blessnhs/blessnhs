#include "ftp.h"
#include "../console.h"
#include "../dynamicMemory.h"
#include "../fat32/fat_filelib.h"
#include "../NETPROTOCOL/socket.h"
#include "../utility.h"
//////////////////////////////////////////////begin ftp

// Map Ftp Command to Handler
FtpCmdHandler ftpCmdHandler[] = { { CMD_OPEN, openCon }, { CMD_LIST, list }, {
		CMD_PUT, put }, { CMD_GET, get }, { CMD_CD, cd }, { CMD_PWD, pwd }, {
		CMD_HASH, hash }, { CMD_QUIT, quit }, { CMD_BYE, bye }, { CMD_SHELL,
		shellEscape }, };

void debug(char *msg) {
	if (mode == MODE_DEBUG) {
		Printf("[debug] : %s \n", msg);
	}
}

int connectServer(char *hostName, short port) {

	hostent *host;
	sockaddr_in serverSock;

	// Check info about remote computer
	if ((host = gethostbyname((char *) hostName)) == NULL) {

		return -1;
	}

	int ncsock;
	// Create socket
	if ((ncsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		Printf("Cant create socket\n");
		return -1;
	}

	// Fill structure sockaddr_in
	// 1) Family of protocols
	serverSock.sin_family = AF_INET;
	// 2) Number of server port
	serverSock.sin_port = htons(port);
	// 3) Setup ip address of server, where we want to connect
	memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);

	net_proto_ip_print(serverSock.sin_addr);

	// Now we are able to connect to remote server
	if (connect(ncsock, (struct sockaddr *) &serverSock, sizeof(serverSock))
			== -1) {
		Printf("Connection cant be estabilished ->\n");
		return -1;
	}
	return ncsock;
}

int connectServer2(char *ip, short port) {

	sockaddr_in serverSock;

	int ncsock;
	// Create socket
	if ((ncsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		Printf("Cant create socket\n");
		return -1;
	}

	// Fill structure sockaddr_in
	// 1) Family of protocols
	serverSock.sin_family = AF_INET;
	// 2) Number of server port
	serverSock.sin_port = htons(port);
	// 3) Setup ip address of server, where we want to connect
	serverSock.sin_addr = net_proto_ip_convert(ip);

	net_proto_ip_print(serverSock.sin_addr);

	// Now we are able to connect to remote server
	if (connect(ncsock, (struct sockaddr *) &serverSock, sizeof(serverSock))
			== -1) {
		Printf("Connection cant be estabilished ->\n");
		return -1;
	}

	return ncsock;
}


static void sendProtocol(int sock, char *protocol) {

	if (send(sock, protocol, strlen(protocol), 0) != strlen(protocol)) {
		Printf("send failed");
		return;
	}
}

static int recvProtocol(int sock, char *recvBuffer, int bufferSize) {
	int recvLen;

	if ((recvLen = recv(sock, recvBuffer, bufferSize - 1, 0)) <= 0) {
		return 0;
	}

	if (recvLen < 0)
		return recvLen;

	recvBuffer[recvLen] = '\0';

	return recvLen;
}

unsigned int downloadFile(int sock,int dptsock, char *filePath, unsigned int fileSize,int hashFlag)
{
	char readBuffer[TEMP_BUFFER_SIZE];
	int readBytes, totalBytes, numHash,cnt;

	Printf("downloadFile %s\n",filePath);

	FL_FILE *fd = fl_fopen(filePath, "wb");
	if(fd == 0)
		return 0;

	 cnt = totalBytes = numHash = 0;
	 while (1)
	 {
		 readBytes = recv(dptsock, readBuffer, TEMP_BUFFER_SIZE, 0);

		 if (readBytes <= 0)
		 {
			break;
		 }

		 cnt = fl_fwrite(readBuffer, 1,readBytes,fd);
		 if(cnt <= 0)
		 {
			 Printf("fl_fwrite fail %d\n",readBytes);
		 }

		 totalBytes += readBytes;

		 Printf("readBytes %d cnt %d fileSize %d totalBytes %d\n",readBytes,cnt,fileSize,totalBytes);

		 if(totalBytes >= (fileSize-1))
			 break;
	 }

	 fl_fclose(fd);

	 Printf("downloadFile end\n");

	 return totalBytes;
}

unsigned int uploadFile(int sock, char *filePath, int hashFlag) {
	/*	char readBuffer[TEMP_BUFFER_SIZE];
	 unsigned int readBytes, totalBytes, numHash;

	 int fd = open(filePath, O_RDONLY);

	 totalBytes = numHash = 0;
	 while ((readBytes = read(fd, readBuffer, TEMP_BUFFER_SIZE)) > 0) {
	 write(sock, readBuffer, readBytes);
	 totalBytes += readBytes;

	 if (hashFlag) {
	 if ((totalBytes/TEMP_BUFFER_SIZE) > numHash) {
	 numHash++;
	 printf("#");
	 }
	 }
	 }
	 close(fd);
	 printf("\n");

	 return totalBytes;*/
}

// sock - PI socket ,  dtpSock - DTP socket
int sock, dtpSock;
int mode = 1;
int hashFlag;


int modeCheck(const char *option) {
	if (!strcmp(option, "-d")) {
		// debug mode
		mode = MODE_DEBUG;
	} else {
		mode = MODE_NORMAL;
	}
	return mode;
}

// initialize ftp client
void initializeFtpClient() {
	hashFlag = 1;
	debug("initialized");

	 ClearScreen();
	 SetCursor( 0, 1 );
}
// ftp client start
void startFtpClient(char *ip, char *port) {
	char cmd[COMMAND_MAX_SIZE];
	memset(cmd,0,COMMAND_MAX_SIZE);

	initializeFtpClient();

	while (1)
	{
		 Printf("ftp>");
		 GUICommandShell(cmd);
		 // call handler
		 commandHandle(cmd);

		 if(strcmp(cmd,"quit") == 0)
			 break;

		memset(cmd,0,COMMAND_MAX_SIZE);

	 }

}

// map command to handler
void commandHandle(char *cmd) {
	int i;
	int numCmd = sizeof(ftpCmdHandler) / sizeof(FtpCmdHandler);

	for (i = 0; i < numCmd; i++) {
		if (!strcmp(cmd, ftpCmdHandler[i].cmd)) {
			(*(ftpCmdHandler[i].handler))(cmd);
			break;
		}
	}
}

void defaultHandler(char *cmd) {
	Printf("default handler: %s\n", cmd);
}

// ftp server connect
void openCon(char *openCmd) {
	char hostname[26], serverPort[16];
	char cmd[BUFFER_SIZE];
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE];

	Printf("\nhostname: ");
	GUICommandShell(hostname);

	Printf("serverPort: ");
	GUICommandShell(serverPort);

	sock = connectServer(hostname, atoi(serverPort));

	int len = recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer,len);

	// send user name
	Printf("\nName: ");
	GUICommandShell(cmd);

	SPrintf(sendBuffer, "User %s\r\n", cmd);
	send(sock, sendBuffer, strlen(sendBuffer), 0);
	//sendProtocol(sock, sendBuffer);
	len = recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer,len);

	// send password
	Printf("\nPassword: ");
	GUICommandShell(cmd);

	SPrintf(sendBuffer, "PASS %s\r\n", cmd);

	sendProtocol(sock, sendBuffer);
	len = recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer,len);

	// get server os information
	memset(sendBuffer, 0, BUFFER_SIZE);
	SPrintf(sendBuffer, "SYST%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	len = recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer,len);

}

// send EPSV or PASS to Server
bool passiveMode(char *ip, int *port) {
	 char sendBuffer[BUFFER_SIZE];
	 char recvBuffer[BUFFER_SIZE];
	 char *findstr;

	 memset(sendBuffer, 0, BUFFER_SIZE);
	 memset(recvBuffer, 0, BUFFER_SIZE);

	 int host0, host1, host2, host3;
	 int port0, port1;

	 SPrintf(sendBuffer, "PASV%s", END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);

	 int passiveLen = 0;

	 {
		 passiveLen = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
		 if(passiveLen <= 0)
			 return false;

		 recvBuffer[passiveLen] = 0;

		 Printf("\n%s\n",recvBuffer);

		 findstr = strstr(recvBuffer,(char*)"227");
	 }

	 int array[10] = {0,};

	 int k=0;
	 int pos = 0;
	 while(1)
	 {
		 if (passiveLen <= pos)
			 return false;

		 if(findstr[pos] == '(')
			 break;

		 pos++;
	 }

	 k=0;
	 pos++;

	 char word[20];
	 int wordindex = 0;
	 while(1)
	 {
		 if (passiveLen <= pos)
			 return false;

		 if (k >= 20)
			 return false;


		 word[k] = findstr[pos];

		 if(word[k] == ',' || word[k] == ')')
		 {
			 char check = word[k];

			 word[k] = 0;
		//	 Printf("%d\n",atoi(word));
			 array[wordindex] = atoi(word);
			 wordindex++;
			 k = 0;

			 if(check == ')')
				 break;
		 }
		 else
		 {
			 k++;
		 }

		 pos++;

		 if(pos >= passiveLen)
			 break;
	 }

	 SPrintf(ip, "%d.%d.%d.%d", array[0], array[1], array[2], array[3]);
	 *port = array[4]*256 + array[5];

	 Printf("\nip %s port %d\n",ip,*port);

	 return true;
}

// get remote working directory file list
void list(char *listCmd) {
	int port;
	int len;
	char ip[16];
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE * 8];

	// recv server response and parsing
	bool ret = passiveMode(ip, &port);
	if(ret == false)
	{
		Printf("Passive Failed\n");
		return ;
	}

	// send LIST command to PI server
	SPrintf(sendBuffer, "LIST%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);

	len = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer,len);

	// recv complete message from PI server
	len = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer,len);

	// connect to DTP
	dtpSock = connectServer2(ip, port);

	// recv file list from DTP
	len = recvProtocol(dtpSock, recvBuffer, BUFFER_SIZE * 8);
	printMessage(recvBuffer,len);

	sclose(dtpSock);

}

// file download
void get(char *getCmd) {
	 int port;
	 unsigned int fileSize;
	 char ip[16], filePath[FILENAME_SIZE], fileName[50];
	 char sendBuffer[BUFFER_SIZE];
	 char recvBuffer[BUFFER_SIZE];

	 memcpy(filePath,"",2);

	 GUICommandShell(fileName);
	 SPrintf(getCmd, "%*s %s%*c", fileName);
	 SPrintf(filePath, "%s/%s", filePath, fileName);

	 debug("get");
	 Printf("fileName: %s\n", fileName);
	 Printf("filePath: %s\n", filePath);

	 if(passiveMode(ip, &port) == false)
	 {
		 Printf("passiveMode Fail\n");
		 return ;
	 }

	 // request server for transfer start - RETR fileName
	 SPrintf(sendBuffer, "SIZE %s%s", fileName, END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 int sizelen = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer,sizelen);

	 char sizebuff[10];
	 memcpy(sizebuff,recvBuffer+3,sizelen-3);
	 sizebuff[9] = 0;
	 fileSize = atoi(sizebuff);
	 //
	 // sscanf(strchr(recvBuffer, '(')+1, "%u", &fileSize);
	 Printf("fileSize: %d\n", atoi(sizebuff));

	 SPrintf(sendBuffer, "TYPE %s%s", "I", END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 sizelen = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer,sizelen);

	 SPrintf(sendBuffer, "REST %s%s", "0", END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 sizelen = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer,sizelen);

	 SPrintf(sendBuffer, "RETR %s%s", fileName, END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE);

	 // connect to DTP
	 dtpSock = connectServer(ip, port);
	 // download file from DTP
	 downloadFile(sock,dtpSock, filePath, fileSize, hashFlag);

	 // recv complete message from PI server
	 sizelen = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer,sizelen);

	 sclose(dtpSock);
}

// file upload
void put(char *putCmd) {
/*	 int port;
	 unsigned int fileSize;
	 char ip[16], filePath[FILENAME_SIZE], fileName[50];
	 char sendBuffer[BUFFER_SIZE];
	 char recvBuffer[BUFFER_SIZE];



	 sscanf(putCmd, "%*s %s%*c", fileName);

	 // get local current working directory
	 //getcwd(filePath, FILENAME_SIZE);

	 memcpy(filePath,"/",2);

	 sscanf(putCmd, "%*s %s%*c", fileName);
	 SPrintf(filePath, "%s/%s", filePath, fileName);

	 debug("put");
	 debug(filePath);

	 passiveMode(ip, &port);

	 // connect to DTP
	 dtpSock = connectServer(ip, port);

	 // request server for transfer start - STOR fileName
	 SPrintf(sendBuffer, "STOR %s%s", fileName, END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer);

	 // file upload to DTP
	 fileSize = uploadFile(dtpSock, filePath, hashFlag);

	 sclose(dtpSock);

	 /// recv complete message from PI server
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer);*/
}

// change remote working directory
void cd(char *cdCmd) {
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE];
	debug("cd");

	GUICommandShell(recvBuffer);
	SPrintf(cdCmd, "%*s %s%*c", recvBuffer);
	debug(recvBuffer);

	SPrintf(sendBuffer, "CWD %s%s", recvBuffer, END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	int len = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer,len);
}

// ftp client exit
void quit(char *quitCmd) {
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE];
	debug("quit");

	SPrintf(sendBuffer, "QUIT%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	int len = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer,len);

	sclose(sock);

}

// same quit
void bye(char *byeCmd) {
	quit(0);
}

// get remote working directory
void pwd(char *pwdCmd) {
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE];
	debug("pwd");

	SPrintf(sendBuffer, "PWD%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	int len = recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer,len);
}

// hash option on/off
void hash(char *hashCmd) {
	debug("hash");
	hashFlag = !hashFlag;

	if (hashFlag == 0) {
		printMessage("hash off\n",8);
	} else {
		printMessage("hash on\n",8);
	}
}

// shell command - not implemented
void shellEscape(char *shellCmd) {
	printMessage("not implemented",15);
}

void printMessage(char *msg,int len)
{
	int i=0;
	while(1)
	{
		if(i == len)
			break;

		Printf("%c", msg[i]);
		i++;
	}
}

//////////////////////////////////////////////end ftp
