#ifndef __FTP_H__
#define __FTP_H__

#include "../types.h"
//begin ftp
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CMD_OPEN "open"
#define	CMD_LIST "ls"
#define CMD_GET "get"
#define CMD_PUT "put"
#define CMD_PWD "pwd"
#define CMD_CD "cd"
#define CMD_QUIT "quit"
#define CMD_BYE "bye"
#define CMD_HASH "hash"
#define CMD_SHELL "!"

#define MODE_DEBUG 1
#define MODE_NORMAL 0

#define FTP_PORT 21

#define TEMP_BUFFER_SIZE 2048

extern int mode;

#define COMMAND_MAX_SIZE 1024
#define BUFFER_SIZE 2048
#define FILENAME_SIZE 256
#define END_OF_PROTOCOL "\r\n"

typedef struct _FtpCmdHandler
{
	char cmd[5];
	void (*handler)(char* arg);

} FtpCmdHandler;



void initializeFtpClient();
void startFtpClient(char *ip, char *port);
void commandHandle(char *cmd);
void defaultHandler(char *cmd);
int modeCheck(const char *option);
void printMessage(char *msg,int len);

int ftpmain ();
void openCon(char *cmd);
void list(char *listCmd);
void get(char *getCmd);
void put(char *putCmd);
void cd(char *cdCmd);
void quit(char *quitCmd);
void pwd(char *pwdCmd);
void bye(char *byeCmd);
void hash(char *hashCmd);
bool passiveMode(char *ip, int *port);
void shellEscape(char *shellCmd);



unsigned int uploadFile(int sock, char *filePath, int hashFlag);

void debug(char *msg);

int connectServer2(char *ip, short port);
int connectServer(char *hostname, short port);
static void sendProtocol(int sock, char *protocol);
static int recvProtocol(int sock, char *recvBuffer, int bufferSize);
unsigned int downloadFile(int socksock,int sock2, char *filePath, unsigned int fileSize, int hashFlag);
unsigned int uploadFile(int sock, char *filePath, int hashFlag);
////////////////////////////////////////////////////////////////////////////////////////////////
//end ftp

#endif
