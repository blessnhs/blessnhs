#include "StdAfx.h"

#include "GSCertification.h"
#include "CertificationProcess.h"



GSCertification::GSCertification(void)
{
	Initialize();
}


GSCertification::~GSCertification(void)
{
}

BOOL GSCertification::Disconnect(GSCLIENT *object)
{
	return TRUE;
}

VOID GSCertification::Accept(GSCLIENT *object)
{

}


BOOL GSCertification::Initialize()
{
	INI.Load();

	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

	GSArgument _arg;

	_arg.m_AliveTime = 30; //sec
	_arg.m_BindPort  = m_Ini.Port;
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
	SetHandler<CertificationProcess>();
	
	return TRUE;
}
