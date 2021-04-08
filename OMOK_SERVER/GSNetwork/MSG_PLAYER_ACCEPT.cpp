#include "StdAfx.h"
#include "MSG_PLAYER_ACCEPT.h"
#include "GSServer.h"

namespace GSNetwork	{


MSG_PLAYER_ACCEPT::MSG_PLAYER_ACCEPT()
{

}

MSG_PLAYER_ACCEPT::~MSG_PLAYER_ACCEPT()
{

}

void MSG_PLAYER_ACCEPT::Execute(LPVOID Param)
{
	if(pClient != NULL)
	{
		SYSLOG().Write("accept socket %d\n",pClient->GetId());

		GSServer::GSServer *pServer = (GSServer::GSServer *)pClient->m_GSServer;

		pClient->SetAliveTime(GetTickCount());

		pServer->Accept(pClient);
	}

}

}