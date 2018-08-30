#include "StdAfx.h"
#include "GSIntergrativeServer.h"
#include "../Ini/GSIniContext.h"
#include "GSIntergrativeProcess.h"

GSIntergrativeServer::GSIntergrativeServer(void)
{
	INI.Load(_T("./UnitySystem.ini"));

	GSArgument _arg;

	_arg.m_AliveTime = 30; //sec
	_arg.m_BindPort  = INI.m_Port;
	_arg.m_DBThreadCnt = 10;
	_arg.m_LogicThreadCnt = 1;
	_arg.m_MaxClient = INI.m_MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
	SetHandler<GSIntergrativeProcess>();

	printf("ServerName [ GSIntergrative ] Port [ %d ] MaxUser [ %d ]\n",INI.m_Port,INI.m_MaxUser);
}


GSIntergrativeServer::~GSIntergrativeServer(void)
{
}

BOOL GSIntergrativeServer::Initialize()
{
	return TRUE;
}

BOOL GSIntergrativeServer::Disconnect(GSCLIENT *object)
{
	return TRUE;
}

VOID GSIntergrativeServer::Accept(GSCLIENT *object)
{

}