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
	//concurrency::concurrent_queue<int> remove_queue;

	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
		//	remove_queue.push(client.first);
			continue;
		}

		if (client.second->GetConnected())
		{
			GSServer::GSServer *pServer = (GSServer::GSServer *)client.second->m_GSServer;

			DWORD client_time = client.second->GetAliveTime();
			DWORD server_check_time = pServer->GetArgument().m_AliveTime * 1000;
			DWORD system_tick = GetTickCount();
			int Diff = system_tick - (client_time + server_check_time);

			if ((client_time + server_check_time) <= system_tick)
				if (client.second->GetType() == _PLAYER_)
					client.second->Close();
		}
	}

	int rcount = 0;
	int count = m_remove_queue.unsafe_size();
	for (int i = 0; i < count; i++)
	{
		GSCLIENT_PTR client;
		if (m_remove_queue.try_pop(client) == true)
		{

			DWORD ClientTime = client->m_DeleteTime;
			DWORD SYSTime = GetTickCount();
			int count = client->GetTCPSocket()->m_SendRefCount;

			int Diff = SYSTime - ClientTime;

			//5분 넘으면 그냥 끊는다.
			if ((ClientTime < SYSTime && count == 0) || (Diff > (1000 * 60 * 5)))
			{
				rcount++;
				m_Clients[client->GetId()] = NULL;

				((GSServer::GSServer*)m_GSServer)->SubPlayerCount(1);
			}
			else
				m_insert_queue.push(client);
		}
	}

	printf("m_remove_queue %d rcount %d \n", count, rcount);

	printf("re insert queue %d\n", m_insert_queue.unsafe_size());
	//다시 넣는다.  ㅠㅠ
	for (int i = 0; i < m_insert_queue.unsafe_size(); i++)
	{
		GSCLIENT_PTR client;
		if (m_insert_queue.try_pop(client) == true)
		{
			m_remove_queue.push(client);
		}
	}
	printf("re m_insert_queue queue %d\n", m_insert_queue.unsafe_size());
	printf("re m_remove_queue queue %d\n", m_remove_queue.unsafe_size());

	
}

int GSClientMgr::GetActiveSocketCount()
{
	int count = 0;

	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			//	remove_queue.push(client.first);
			continue;
		}

		if (client.second->GetConnected() == FALSE)
			continue;
		
		count++;
	}

	return count;
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

	if (client->second == NULL)
		return FALSE;

	//삭제 처리는 8초후에 일괄로처리한다. 
	client->second->m_DeleteTime = GetTickCount() + 80000;
	m_remove_queue.push(client->second);
	

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