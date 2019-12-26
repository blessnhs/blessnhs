#include "StdAfx.h"
#include "GSClientMgr.h"
#include "GSServer.h"
#include <boost/make_shared.hpp>

namespace GSNetwork	{	namespace GSClientMgr	{

GSClientMgr::GSClientMgr(void)
{
}


GSClientMgr::~GSClientMgr(void)
{
}


VOID GSClientMgr::CheckAliveTime()
{
	for each (auto client in m_Clients)
	{
		if (client.second->GetConnected())
		{
			GSServer::GSServer *pServer = (GSServer::GSServer *)client.second->m_GSServer;

			if(client.second->GetAliveTime() + (pServer->GetArgument().m_AliveTime * 1000) <= GetTickCount())
				if(client.second->GetType() == _PLAYER_ )
					client.second->OnDisconnect();
		}
	}
}

int GSClientMgr::GetActiveSocketCount()
{
	int Count = 0;
	for each (auto client in m_Clients)
	{
		if (client.second->GetConnected() == FALSE)
		{
			Count++;
		}
	}

	return Count;
}

BOOL GSClientMgr::Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID pServer)
{
	if (!ListenSocket)
		return FALSE;

	DWORD MaxUser = MaxClients;

	for (DWORD i=0;i<MaxUser;i++)
	{
		GSCLIENT_PTR pClient = boost::make_shared<GSClient>();
		pClient->Create(TCP);
		pClient->m_GSServer = pServer;

		if (!pClient->GetTCPSocket()->Initialize())
		{
			End();
			return FALSE;
		}

		if (!pClient->GetTCPSocket()->Accept(ListenSocket))
		{
			End();
			return FALSE;
		}

		pClient->SetId(i);
		pClient->SetType(_PLAYER_);
		m_Clients[i] = pClient;

	}

	return TRUE;
}

VOID GSClientMgr::End()
{

}

}	}