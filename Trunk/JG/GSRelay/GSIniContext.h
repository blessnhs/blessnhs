#pragma once

#include "SingleTonHolder.h"
#include "Common.h"

namespace GSRelayServer	{	namespace GSIniContext	{

class GSIniContext
{
public:
	GSIniContext(void);
	~GSIniContext(void);

	DWORD m_Port;
	DWORD m_Id;
	WCHAR m_Name[SERVER_NAME];
	DWORD m_MaxPlayerCount;

	DWORD m_UdpPort;
	DWORD m_UdpCount;

	WCHAR GSIP[MAX_IP_ADDRESS];
	DWORD GSPort;

	VOID Load(WCHAR *IniName);

};

}	}

#define INICONTEXT Singleton<GSRelayServer::GSIniContext::GSIniContext>::Instance()

