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
		if (client.second == NULL)
			continue;

		if (client.second->GetConnected())
		{
			GSServer::GSServer *pServer = (GSServer::GSServer *)client.second->m_GSServer;

			if (client.second->GetAliveTime() + (pServer->GetArgument().m_AliveTime * 1000) <= GetTickCount())
				if (client.second->GetType() == _PLAYER_)
					//client.second->OnDisconnect(client.second);
					;//client.second->Close();
		}
	}
}

int GSClientMgr::GetActiveSocketCount()
{
	int Count = 0;
	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
			continue;

		if (client.second->GetConnected() == FALSE)
		{
			Count++;
		}
	}

	return Count;
}
int GSClientMgr::IncClientId()
{
	static atomic<int> intAtomic = 0;

	int idx = intAtomic.fetch_add(1);         

	return idx;
}


GSCLIENT_PTR GSClientMgr::GetClient(int id)
{
	auto client = m_Clients.find(id);

	if (client == m_Clients.end())
		return NULL;

	return client->second;
}

BOOL GSClientMgr::DelClient(int id)
{
	auto client = m_Clients.find(id);

	if (client == m_Clients.end())
		return FALSE;

	m_Clients[id] = NULL;

	return TRUE;
}

BOOL GSClientMgr::AddClient(GSCLIENT_PTR pClient)
{
	m_Clients[pClient->GetId()] = pClient;

	return TRUE;
}

BOOL GSClientMgr::NewClient(SOCKET ListenSocket, LPVOID pServer)
{
	CThreadSync Sync;

	if (!ListenSocket)
		return FALSE;

	int NewClient = 1;
	if (GetActiveSocketCount() < 20)
	{
		NewClient = 100;
		printf("New Alloc  %d \n", NewClient);
	}

	for (DWORD i = 0; i < NewClient; i++)
	{
		GSCLIENT_PTR pClient = boost::make_shared<GSClient>();
		pClient->SetId(IncClientId());
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

		pClient->SetType(_PLAYER_);
		m_Clients[pClient->GetId()] = pClient;

	}

	return TRUE;
}

BOOL GSClientMgr::Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID pServer)
{
	if (!ListenSocket)
		return FALSE;

	DWORD MaxUser = MaxClients;

	for (DWORD i=0;i<MaxUser;i++)
	{
		GSCLIENT_PTR pClient = boost::make_shared<GSClient>();
		pClient->SetId(IncClientId());
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

		pClient->SetType(_PLAYER_);
		m_Clients[pClient->GetId()] = pClient;

	}

	return TRUE;
}

VOID GSClientMgr::End()
{

}

}	}