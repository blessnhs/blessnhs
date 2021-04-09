#include "StdAfx.h"
#include "INI.h"

VOID HubINI::Load()
{
	Port = GetPrivateProfileInt(("SERVER"), ("PORT"), 1982,("./Hub.ini"));
	MaxUser = GetPrivateProfileInt(("SERVER"), ("MAXUSER"), 100,("./Hub.ini"));


	CHAR Default_Version[10];
	GetPrivateProfileString(("SERVER"), ("VERSION"), ("1.0"), Default_Version, sizeof(Default_Version), ("./Hub.ini"));

	Version = atof(Default_Version);

	Default_FrontPort = GetPrivateProfileInt(("SERVER"), ("DEFAULT_FRONT_PORT"), 100,("./Hub.ini"));

	AliveSeconds = GetPrivateProfileInt(("SERVER"), ("ALIVE_SECONDS"), 60 * 60 /*1HOURS*/, ("./Hub.ini"));

	GetPrivateProfileString(("SERVER"), ("DEFAULT_FRONT_IP"),("127.0.0.1"),Default_FrontIp,sizeof(Default_FrontIp),("./Hub.ini"));

	GetPrivateProfileString(("SERVER"), ("ODBC_SYSTEM32_NAME"), ("."), ODBC_NAME, sizeof(ODBC_NAME), ("./Hub.ini"));
	GetPrivateProfileString(("SERVER"), ("ODBC_ID"), ("."), ODBC_ID, sizeof(ODBC_ID), ("./Hub.ini"));
	GetPrivateProfileString(("SERVER"), ("ODBC_PASSWD"), ("."), ODBC_PWD, sizeof(ODBC_PWD), ("./Hub.ini"));

	printf("[ Hub ] Port [ %d ] MaxUser [ %d ] Alive Seconds[ %d ] Version [%f]\n",Port,MaxUser, AliveSeconds, Version);

	//printf("ODBC SYSTEM32 NAME [%s] ID [%s] PWD [%s] \n", ODBC_NAME, ODBC_ID, ODBC_PWD);
}