#pragma once

class FrontINI : public Singleton<FrontINI>
{
public:
	FrontINI(void) {} 
	~FrontINI(void) {}

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

	CHAR HubIp[MAX_IP_ADDRESS];
	WORD  HubPort;
	WORD  HubSessionCount;

};

#define  INI Singleton<FrontINI>::Instance()
