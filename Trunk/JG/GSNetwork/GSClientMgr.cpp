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
	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;

	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetConnected())
		{
			DWORD client_time = client.second->GetAliveTime();
			DWORD server_check_time = pServer->GetArgument().m_AliveTime * 1000;
			DWORD system_tick = GetTickCount();
			int Diff = system_tick - (client_time + server_check_time);

			if ((client_time + server_check_time) <= system_tick)
				if (client.second->GetType() == _PLAYER_)
				{
					client.second->OnDisconnect(client.second);
					client.second->Close();
				}
		}
	}

	for (int i = 0; i < m_Remove_Queue.unsafe_size(); i++)
	{
		GSCLIENT_PTR client;
		if (m_Remove_Queue.try_pop(client) == true)
		{

			DWORD ClientTime = client->m_DeleteTime;
			DWORD SYSTime = GetTickCount();
			int count = client->GetTCPSocket()->m_SendRefCount;

			int Diff = SYSTime - ClientTime;

			//1분 넘으면 그냥 끊는다.
			if ((ClientTime < SYSTime && count == 0) || (Diff > (1000 * 60 * 1)))
			{
				if (DelClient(client->GetId()) == FALSE)
				{
					printf("DelClient is not 0 failed \n");
				}
			}
			else
				m_ReInsert_Queue.push(client);

			if ((ClientTime < SYSTime) && count != 0)
			{
				printf("count is not 0 %d \n", count);
			}
		}
	}

	//다시 넣는다.  ㅠㅠ
	for (int i = 0; i < m_ReInsert_Queue.unsafe_size(); i++)
	{
		GSCLIENT_PTR client;
		if (m_ReInsert_Queue.try_pop(client) == true)
		{
			m_Remove_Queue.push(client);
		}
	}
		
	printf("\nre m_insert_queue queue %d\n", m_ReInsert_Queue.unsafe_size());
	printf("re m_remove_queue queue %d\n", m_Remove_Queue.unsafe_size());
	printf("\nconnect socket count %d GetActiveSocketCount %d ConnectableSocketCount %d\n", 
		pServer->CurrentPlayerCount(), GetActiveSocketCount(), ConnectableSocketCount());
}

int  GSClientMgr::ConnectableSocketCount()
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;

	int count = m_MaxClients - pServer->CurrentPlayerCount();

	/*for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			int a = 0;
			continue;
		}

		if (client.second->GetConnected() == TRUE)
		{
			int a = 0;

			continue;
		}

		if (client.second->m_DeleteTime > 0)
		{
			int a = 0;
			continue;
		}

		count++;
	}*/

	return count;
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
	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetId() == id)
			return client.second;
	}

	return NULL;
}

BOOL GSClientMgr::AddClient(GSCLIENT_PTR newclient)
{
	CThreadSync sync;

	bool find = false;

	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			m_Clients[client.first] = newclient;
			return TRUE;
		}
	}

	if (find == false)
	{
		m_Clients[newclient->GetId()] = newclient;
		return TRUE;
	}

	return FALSE;
}

BOOL GSClientMgr::BookDelClient(int id)
{
	auto client = GetClient(id);

	if (client == NULL)
		return FALSE;

	//삭제 처리는 5초후에 일괄로처리한다. 
	client->m_DeleteTime = GetTickCount() + 50000;
	m_Remove_Queue.push(client);
	

	return TRUE;
}

BOOL GSClientMgr::DelClient(int id)
{
	for each (auto client in m_Clients)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetId() == id)
		{
			m_Clients[client.first] = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GSClientMgr::NewClient(SOCKET ListenSocket, LPVOID pServer)
{
	if (!ListenSocket)
		return FALSE;

	int NewClient = 1;
	if (ConnectableSocketCount() < 20)
	{
		NewClient = 100;
		printf("New Alloc  %d \n", NewClient);

		m_MaxClients += 100;
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
		
		if (AddClient(pClient) == FALSE)
		{
			printf("NewClient failed \n");
		}
	}

	return TRUE;
}

BOOL GSClientMgr::Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID pServer)
{
	if (!ListenSocket)
		return FALSE;

	DWORD MaxUser = m_MaxClients = MaxClients;

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