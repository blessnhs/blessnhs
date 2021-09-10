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
	_arg.m_AliveTime = m_Ini.AliveSeconds * 1000;

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	Create(_arg);

	//default
	//
	SetHandler<IUdpProcess>();
	
	return TRUE;
}
