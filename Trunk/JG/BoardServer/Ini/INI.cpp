#include "StdAfx.h"
#include "INI.h"

VOID BoardINI::Load()
{
	Port = GetPrivateProfileInt(("SERVER"), ("PORT"), 1982,("./Board.ini"));
	MaxUser = GetPrivateProfileInt(("SERVER"), ("MAXUSER"), 100,("./Board.ini"));

	Default_FrontPort = GetPrivateProfileInt(("SERVER"), ("DEFAULT_FRONT_PORT"), 100,("./Board.ini"));

	GetPrivateProfileString(("SERVER"), ("DEFAULT_FRONT_IP"),("127.0.0.1"),Default_FrontIp,sizeof(Default_FrontIp),("./Board.ini"));

	printf("[ Board ] Port [ %d ] MaxUser [ %d ] Front Ip = %s:%d \n",Port,MaxUser,Default_FrontIp,Default_FrontPort);
}