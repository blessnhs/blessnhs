#pragma once

#include "SingleTonHolder.h"

class GSIniContext : public Singleton<GSIniContext>
{
public:
	GSIniContext(void);
	~GSIniContext(void);

	DWORD m_Port;
	DWORD m_MaxUser;
	DWORD m_Id;
	WCHAR m_Name[SERVER_NAME];

	WCHAR m_FontIp[20];
	WORD m_FrontPort;

	VOID Load(WCHAR *IniName);

};

#define  INI Singleton<GSIniContext>::Instance()