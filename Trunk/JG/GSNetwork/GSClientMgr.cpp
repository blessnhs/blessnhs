#include "StdAfx.h"
#include "GSClientMgr.h"
#include "GSServer.h"
#include "ConsoleHelper.h"
#include <boost/make_shared.hpp>

extern atomic<int>		DebugCount;
extern atomic<int>		ConnectCount;
extern atomic<int>		DisConnectCount;

atomic<int>				NewConnectount = 0;

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

	for each (auto client in m_ClientsForLoop)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetConnected())
		{
			DWORD client_time = client.second->GetAliveTime();
			DWORD server_check_time = pServer->GetArgument().m_AliveTime;
			DWORD system_tick = GetTickCount();
			int Diff = system_tick - (client_time + server_check_time);

			if ((client_time + server_check_time) <= system_tick)
				if (client.second->GetType() == _PLAYER_)
				{
				//	client.second->OnDisconnect(client.second);
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
			int count = client->GetTCPSocket()->m_OLP_REMAIN_COUNT_ACC + client->GetTCPSocket()->m_OLP_REMAIN_COUNT_REC + client->GetTCPSocket()->m_OLP_REMAIN_COUNT_SND;

			int Diff = SYSTime - ClientTime;

			//1분 넘으면 그냥 삭제한다.
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


	//SYSTEMTIME		sysTime;
	//::GetLocalTime(&sysTime);

	//printf("[ %04d-%02d-%02d %02d:%02d:%02d ] Current User Count %d connectable count %d Debug Count %d\n",
	//	sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, pServer->CurrentPlayerCount(), ConnectableSocketCount(), DebugCount.fetch_add(0));

//	printf("\nre m_insert_queue queue %d\n", m_ReInsert_Queue.unsafe_size());
//	printf("re m_remove_queue queue %d\n", m_Remove_Queue.unsafe_size());
//	printf("\nconnect socket count %d GetActiveSocketCount %d ConnectableSocketCount %d\n", 
//	pServer->CurrentPlayerCount(), GetActiveSocketCount(), ConnectableSocketCount());

	//printf("[UserCount : %d] [Debug Count : %d]\n",
	//	pServer->CurrentPlayerCount(), DebugCount.fetch_add(0));
	{

		char msg[256];
		
		sprintf(msg,"[Conncted Socket : %d] [Debug Count : %d] [Total Connect : %d] [Ttoal NewConnect : %d][Total Disconnect %d]\n",
				pServer->CurrentPlayerCount(), DebugCount.fetch_add(0), ConnectCount.fetch_add(0), NewConnectount.fetch_add(0), DisConnectCount.fetch_add(0));

		ConsoleHelper::DebugConsoleString(0, msg);

	}
}

int  GSClientMgr::ConnectableSocketCount()
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;

	int count = m_MaxClients - pServer->CurrentPlayerCount();

	return count;
}

int GSClientMgr::GetActiveSocketCount()
{
	int count = 0;

	for each (auto client in m_ClientsForLoop)
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

GSCLIENT_PTR GSClientMgr::GetClientLoop(int id)
{

	for each (auto client in m_ClientsForLoop)
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


GSCLIENT_PTR GSClientMgr::GetClient(int id)
{
	auto find = m_Clients.find(id);
	if (find == m_Clients.end())
		return NULL;

	return m_Clients[id];

}

BOOL GSClientMgr::AddClientLoop(GSCLIENT_PTR newclient)
{
	bool find = false;

	for each (auto client in m_ClientsForLoop)
	{
		if (client.second == NULL)
		{
			m_ClientsForLoop[client.first] = newclient;
			return TRUE;
		}
	}

	if (find == false)
	{
		m_ClientsForLoop[newclient->GetId()] = newclient;
		return TRUE;
	}

	return FALSE;
}

BOOL GSClientMgr::DelClientLoop(int id)
{
	for each (auto client in m_ClientsForLoop)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetId() == id)
		{
			m_ClientsForLoop[client.first] = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GSClientMgr::AddClient(GSCLIENT_PTR newclient)
{
	CThreadSync sync;

	m_Clients[newclient->GetId()] = newclient;

	AddClientLoop(newclient);

	return TRUE;
}

BOOL GSClientMgr::BookDelClient(int id)
{
	auto client = GetClient(id);

	if (client == NULL)
		return FALSE;

	//삭제 처리는 5초후에 일괄로처리한다. 
	client->m_DeleteTime = GetTickCount() + 5000;
	m_Remove_Queue.push(client);
	

	return TRUE;
}

BOOL GSClientMgr::DelClient(int id)
{
	m_Clients[id] = NULL;

	DelClientLoop(id);

	return TRUE;
}

BOOL GSClientMgr::NewClient(SOCKET ListenSocket, LPVOID pServer)
{
	if (!ListenSocket)
	{
		printf("NewClient !ListenSocket \n");
		return FALSE;
	}

	CThreadSync Sync;

	NewConnectount.fetch_add(1);

	int NewClient = 1;
	if (ConnectableSocketCount() < 20)
	{
		NewClient = 100;
		printf("Resize Client  %d \n", NewClient);

		m_MaxClients += NewClient;
	}
	
	for (DWORD i = 0; i < NewClient; i++)
	{
		GSCLIENT_PTR pClient = boost::make_shared<GSClient>();
		pClient->SetId(IncClientId());
		pClient->Create(TCP);
		pClient->m_GSServer = pServer;

		pClient->SetType(_PLAYER_);

		if (AddClient(pClient) == FALSE)
		{
			printf("NewClient failed...1 \n");
		}


		if (!pClient->GetTCPSocket()->Initialize())
		{
			printf("NewClient failed...2 \n");
			DelClient(pClient->GetId());
			End();
			return FALSE;
		}

		if (!pClient->GetTCPSocket()->Accept(ListenSocket))
		{
			printf("NewClient failed...3 \n");
			DelClient(pClient->GetId());
			End();
			return FALSE;
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
		AddClient(pClient);
	}

	return TRUE;
}

VOID GSClientMgr::End()
{

}

}	}