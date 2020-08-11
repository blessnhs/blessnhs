#include "StdAfx.h"
#include "INI.h"

VOID BoardINI::Load()
{
	Port = GetPrivateProfileInt(("SERVER"), ("PORT"), 1982,("./Board.ini"));
	MaxUser = GetPrivateProfileInt(("SERVER"), ("MAXUSER"), 100,("./Board.ini"));

	Default_FrontPort = GetPrivateProfileInt(("SERVER"), ("DEFAULT_FRONT_PORT"), 100,("./Board.ini"));

	GetPrivateProfileString(("SERVER"), ("DEFAULT_FRONT_IP"),("127.0.0.1"),Default_FrontIp,sizeof(Default_FrontIp),("./Board.ini"));

	GetPrivateProfileString(("SERVER"), ("ODBC_SYSTEM32_NAME"), ("."), ODBC_NAME, sizeof(ODBC_NAME), ("./Board.ini"));
	GetPrivateProfileString(("SERVER"), ("ODBC_ID"), ("."), ODBC_ID, sizeof(ODBC_ID), ("./Board.ini"));
	GetPrivateProfileString(("SERVER"), ("ODBC_PASSWD"), ("."), ODBC_PWD, sizeof(ODBC_PWD), ("./Board.ini"));

	printf("[ Board ] Port [ %d ] MaxUser [ %d ] \n",Port,MaxUser);

	//printf("ODBC SYSTEM32 NAME [%s] ID [%s] PWD [%s] \n", ODBC_NAME, ODBC_ID, ODBC_PWD);
}