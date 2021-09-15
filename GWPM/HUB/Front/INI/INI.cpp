#include "StdAfx.h"
#include "INI.h"

VOID FrontINI::Load()
{
	std::string Name = "./Front.ini";
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


	GetPrivateProfileString(("SERVER"), ("HUB_IP"), ("127.0.0.1"), HubIp, sizeof(HubIp), (Name.c_str()));
	HubPort = GetPrivateProfileInt(("SERVER"), ("HUB_PORT"), 100, (Name.c_str()));

	HubSessionCount = GetPrivateProfileInt(("SERVER"), ("HUB_SESSION_COUNT"), 10, (Name.c_str()));


	printf("\n[ Front ] Port [ %d ] MaxUser [ %d ] Alive Seconds[ %d ] Version [%f]\n[Hub Ip] [ %s ] [ Hub Port ][ %d]\n", Port, MaxUser, AliveSeconds, Version, HubIp, HubPort);
}