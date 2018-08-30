#include "StdAfx.h"
#include "GSClientMgr.h"
#include "GSServer.h"

namespace GSNetwork	{	namespace GSClientMgr	{

GSClientMgr::GSClientMgr(void)
{
}


GSClientMgr::~GSClientMgr(void)
{
}


VOID GSClientMgr::CheckAliveTime()
{
	for (DWORD i=0;i<m_Clients.size();i++)
	{
		if (m_Clients[i]->GetConnected())
		{
			GSServer::GSServer *pServer = (GSServer::GSServer *)m_Clients[i]->m_GSServer;

			if(m_Clients[i]->GetAliveTime() + (pServer->GetArgument().m_AliveTime * 1000) <= GetTickCount())
				if( m_Clients[i]->GetType() == _PLAYER_ )
					m_Clients[i]->OnDisconnect();
		}
	}
}

int GSClientMgr::GetActiveSocketCount()
{
	int Count = 0;
	for (DWORD i=0;i<m_Clients.size();i++)
	{
		if (m_Clients[i]->GetConnected() == FALSE)
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
		GSCLIENT *pClient = new GSCLIENT();
		pClient->Create(TCP);
		m_Clients.push_back(pClient);
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
	}

	return TRUE;
}

VOID GSClientMgr::End()
{

}

}	}