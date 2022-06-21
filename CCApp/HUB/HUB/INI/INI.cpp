#include "StdAfx.h"
#include "INI.h"

VOID HubINI::Load()
{
	std::string Name = "./Hub.ini";
	Port = GetPrivateProfileInt(("SERVER"), ("PORT"), 1982, (Name.c_str()));
	MaxUser = GetPrivateProfileInt(("SERVER"), ("MAXUSER"), 100, (Name.c_str()));


	CHAR Default_Version[10];
	GetPrivateProfileString(("SERVER"), ("VERSION"), ("1.0"), Default_Version, sizeof(Default_Version), (Name.c_str()));

	Version = atof(Default_Version);

	Default_FrontPort = GetPrivateProfileInt(("SERVER"), ("DEFAULT_FRONT_PORT"), 100, (Name.c_str()));

	AliveSeconds = GetPrivateProfileInt(("SERVER"), ("ALIVE_SECONDS"), 60 * 60 /*1HOURS*/, (Name.c_str()));

	GetPrivateProfileString(("SERVER"), ("DEFAULT_FRONT_IP"), ("127.0.0.1"), Default_FrontIp, sizeof(Default_FrontIp), (Name.c_str()));

	GetPrivateProfileString(("SERVER"), ("ODBC_SYSTEM32_NAME"), ("."), ODBC_NAME, sizeof(ODBC_NAME), (Name.c_str()));
	GetPrivateProfileString(("SERVER"), ("ODBC_ID"), ("."), ODBC_ID, sizeof(ODBC_ID), (Name.c_str()));
	GetPrivateProfileString(("SERVER"), ("ODBC_PASSWD"), ("."), ODBC_PWD, sizeof(ODBC_PWD), (Name.c_str()));

	printf("\n[ Hub ] Port [ %d ] MaxUser [ %d ] Alive Seconds[ %d ] Version [%f]\n", Port, MaxUser, AliveSeconds, Version);
}