#pragma once

class HubINI : public Singleton<HubINI>
{
public:
	HubINI(void) {} 
	~HubINI(void) {}

	VOID Load();

	CHAR Default_FrontIp[MAX_IP_ADDRESS];
	WORD  Default_FrontPort;

	DWORD Port ; 
	DWORD MaxUser ;

	CHAR ODBC_NAME[MAX_CHAR_LENGTH];
	CHAR ODBC_ID[MAX_CHAR_LENGTH];
	CHAR ODBC_PWD[MAX_CHAR_LENGTH];

	INT AliveSeconds;

	float Version;

};

#define  INI Singleton<HubINI>::Instance()
