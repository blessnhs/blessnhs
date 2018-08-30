#pragma once
#include "common.h"

#include "SingleTonHolder.h"

namespace GSFrontServer	{	namespace GSIniContext	{

class GSIniContext : public Singleton<GSIniContext>
{
public:
	GSIniContext(void);
	~GSIniContext(void);

	DWORD m_Port;
	DWORD m_MaxUser;
	DWORD m_Id;
	WCHAR m_Name[SERVER_NAME];

	WCHAR m_DSNGameDB[MAX_INI_STRING_LENGTH];
	WCHAR m_DSNAccount[MAX_INI_STRING_LENGTH];
	WCHAR m_Account[MAX_INI_STRING_LENGTH];
	WCHAR m_Passwd[MAX_INI_STRING_LENGTH];

	WCHAR m_UnityServerIp[MAX_IP_ADDRESS];
	DWORD m_UnityPort;

	VOID Load(WCHAR *IniName);

};

}	}

#define  INI Singleton<GSFrontServer::GSIniContext::GSIniContext>::Instance()
