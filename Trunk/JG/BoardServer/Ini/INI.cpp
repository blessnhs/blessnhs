#include "StdAfx.h"
#include "INI.h"

VOID BoardINI::Load()
{
	Port = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 1982,_T("./Board.ini"));
	MaxUser = GetPrivateProfileInt(_T("SERVER"), _T("MAXUSER"), 100,_T("./Board.ini"));

	Default_FrontPort = GetPrivateProfileInt(_T("SERVER"), _T("DEFAULT_FRONT_PORT"), 100,_T("./Board.ini"));

	GetPrivateProfileString(_T("SERVER"), _T("DEFAULT_FRONT_IP"),_T("127.0.0.1"),Default_FrontIp,sizeof(Default_FrontIp),_T("./Board.ini"));

	printf("[ Board ] Port [ %d ] MaxUser [ %d ] Front Ip = %S:%d \n",Port,MaxUser,Default_FrontIp,Default_FrontPort);
}