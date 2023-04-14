#include "stdafx.h"

#include "GSFront.h"
#include "FrontProcess.h"
#include "IUdpProcess.h"
#include "../DB/DBProxy/DBProcessContainer.h"

class RequestDeleteAllConcurrentUser;

GSFront::GSFront(void)
{
}


GSFront::~GSFront(void)
{
}

BOOL GSFront::Disconnect(GSCLIENT_PTR pSession)
{
	if (pSession->GetClientType() == SERVER_CLIENT)
	{
		AllClose();


		PROXYHUB.Del(pSession);
		PROXYHUB.Create(1);



	}
	else if(pSession->GetClientType() == CLIENT)
	{
		auto hub = PROXYHUB.GetHub(pSession->GetId());
		if (hub != NULL)
		{
			CLIENT_LOGOUT_REQ res;
			SEND_PROTO_BUFFER( res, hub, pSession->GetId())
		}
	
	}

	return TRUE;
}

VOID GSFront::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<FrontProcess>();
}


BOOL GSFront::Initialize()
{
	INI.Load();

	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	WORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

	GSArgument _arg;

	_arg.m_BindPort  = m_Ini.Port;
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;
	if (INI.AliveSeconds != -1)
		_arg.m_AliveTime = INI.AliveSeconds * 1000;
	else
		_arg.m_AliveTime = 0;

	_arg.m_UseCompress = false; //릴레이서버 그냥 릴레이만 한다.

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	Create(_arg);

	//default
	//
	SetHandler<IUdpProcess>();
	
	return TRUE;
}
