#include "ftp.h"
#include "../NETPROTOCOL/socket.h"
#include "../console.h"
#include "../dynamicMemory.h"

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

static int connectServer(char *hostName, short port) {

	hostent *host;
	sockaddr_in serverSock;

	Printf("hostName %s\n", hostName);

	// Check info about remote computer
	if ((host = gethostbyname((char *) hostName)) == NULL) {
		Printf("tftp -> wrong address\n");
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

static int connectServer2(char *ip, short port) {

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

	if (MODE_DEBUG == mode) {
		Printf("send: %s", protocol);
	}
}

static void recvProtocol(int sock, char *recvBuffer, int bufferSize) {
	int recvLen;

	if ((recvLen = recv(sock, recvBuffer, bufferSize - 1, 0)) <= 0) {
		Printf("recv failed");
		return;
	}

	recvBuffer[recvLen] = '\0';

	if (MODE_DEBUG == mode) {
		Printf("recv: %s", recvBuffer);
	}
}

unsigned int downloadFile(int sock, char *filePath, unsigned int fileSize,
		int hashFlag) {
	/*	char readBuffer[TEMP_BUFFER_SIZE];
	 unsigned int readBytes, totalBytes, numHash;

	 int fd = open(filePath, O_WRONLY | O_CREAT, 0744);

	 totalBytes = numHash = 0;
	 while (totalBytes < fileSize) {
	 if ((readBytes = read(sock, readBuffer, TEMP_BUFFER_SIZE)) <= 0) {
	 close(fd);
	 return totalBytes;
	 }
	 write(fd, readBuffer, readBytes);
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

int ftpmain() {
	startFtpClient("", "");

	return 0;
}

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
}

// ftp client start
void startFtpClient(char *ip, char *port) {
	char cmd[COMMAND_MAX_SIZE];

	initializeFtpClient();

	while (1)
	{
		// input user command
		 if (ip == 0 && port == 0)
		 {
			 printMessage("ftp>");
			 gets_s(cmd, COMMAND_MAX_SIZE);
		 }
		 else
		 {
			 SPrintf(cmd, "open %s %s", ip, port);
			 ip = port = 0;
		 }
		 // call handler
		 commandHandle(cmd);
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
	;
	char recvBuffer[BUFFER_SIZE];
	;

	Printf("hostname: ");
	gets_s(hostname, 25);

	Printf("serverPort: ");
	gets_s(serverPort, 15);

	sock = connectServer(hostname, atoi(serverPort));

	recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);

	// send user name
	Printf("Name: ");
	gets_s(cmd, COMMAND_MAX_SIZE);

	SPrintf(sendBuffer, "User %s\r\n", cmd);
	send(sock, sendBuffer, strlen(sendBuffer), 0);
	//sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer);

	// send password
	Printf("Password: ");
	gets_s(cmd, COMMAND_MAX_SIZE);

	SPrintf(sendBuffer, "PASS %s\r\n", cmd);

	sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer);

	// get server os information
	memset(sendBuffer, 0, BUFFER_SIZE);
	SPrintf(sendBuffer, "SYST%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE - 1);
	printMessage(recvBuffer);

}

// send EPSV or PASS to Server
void passiveMode(char *ip, int *port) {
	 char sendBuffer[BUFFER_SIZE];
	 char recvBuffer[BUFFER_SIZE];
	 int host0, host1, host2, host3;
	 int port0, port1;

	 SPrintf(sendBuffer, "PASV%s", END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE-1);
	 printMessage(recvBuffer);


	 int array[6] = {0,};

	 int k=0;
	 int pos = 0;
	 while(1)
	 {
		 if(recvBuffer[pos] == '(')
			 break;

		 pos++;
	 }

	 k=0;
	 pos++;

	 Printf("passiveMode %d\n",pos);
	 char word[10];
	 int wordindex = 0;
	 while(1)
	 {
		 word[k] = recvBuffer[pos];

		 if(word[k] == ',' || word[k] == ')')
		 {
			 char check = word[k];

			 word[k] = 0;
			 Printf("%d\n",atoi(word));
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
	 }

	 SPrintf(ip, "%d.%d.%d.%d", array[0], array[1], array[2], array[3]);
	 *port = array[4]*256 + array[5];

	 Printf("dtp port : %d ip %s\n", *port,ip);

}

// get remote working directory file list
void list(char *listCmd) {
	int port;
	char ip[16];
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE * 8];

	debug("list");

	// recv server response and parsing
	passiveMode(ip, &port);

	// connect to DTP
	dtpSock = connectServer2(ip, port);

	// send LIST command to PI server
	SPrintf(sendBuffer, "LIST%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer);

	// recv file list from DTP
	recvProtocol(dtpSock, recvBuffer, BUFFER_SIZE * 8);
	printMessage(recvBuffer);

	// recv complete message from PI server
	recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer);

	sclose(dtpSock);
}

// file download
void get(char *getCmd) {
	/*	int port;
	 unsigned int fileSize;
	 char ip[16], filePath[FILENAME_SIZE], fileName[50];
	 char sendBuffer[BUFFER_SIZE];
	 char recvBuffer[BUFFER_SIZE];

	 // get local current working directory
	 //getcwd(filePath, FILENAME_SIZE);


	 memcpy(filePath,"/",2);

	 sscanf(getCmd, "%*s %s%*c", fileName);
	 SPrintf(filePath, "%s/%s", filePath, fileName);

	 debug("get");
	 Printf("fileName: %s\n", fileName);
	 Printf("filePath: %s\n", filePath);

	 passiveMode(ip, &port);

	 // connect to DTP
	 dtpSock = connectServer(ip, port);

	 // request server for transfer start - RETR fileName
	 SPrintf(sendBuffer, "RETR %s%s", fileName, END_OF_PROTOCOL);
	 sendProtocol(sock, sendBuffer);
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer);

	 // extract fileSize
	 sscanf(strchr(recvBuffer, '(')+1, "%u", &fileSize);
	 Printf("fileSize: %u\n", fileSize);

	 // download file from DTP
	 downloadFile(dtpSock, filePath, fileSize, hashFlag);

	 // recv complete message from PI server
	 recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	 printMessage(recvBuffer);

	 sclose(dtpSock);*/
}

// file upload
void put(char *putCmd) {
	/*	int port;
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

	gets_s(recvBuffer, BUFFER_SIZE);
	SPrintf(cdCmd, "%*s %s%*c", recvBuffer);
	debug(recvBuffer);

	SPrintf(sendBuffer, "CWD %s%s", recvBuffer, END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer);
}

// ftp client exit
void quit(char *quitCmd) {
	char sendBuffer[BUFFER_SIZE];
	char recvBuffer[BUFFER_SIZE];
	debug("quit");

	SPrintf(sendBuffer, "QUIT%s", END_OF_PROTOCOL);
	sendProtocol(sock, sendBuffer);
	recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer);

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
	recvProtocol(sock, recvBuffer, BUFFER_SIZE);
	printMessage(recvBuffer);
}

// hash option on/off
void hash(char *hashCmd) {
	debug("hash");
	hashFlag = !hashFlag;

	if (hashFlag == 0) {
		printMessage("hash off\n");
	} else {
		printMessage("hash on\n");
	}
}

// shell command - not implemented
void shellEscape(char *shellCmd) {
	printMessage("not implemented");
}

void printMessage(char *msg) {
	Printf("\n[%s]\n", msg);
}

//////////////////////////////////////////////end ftp
