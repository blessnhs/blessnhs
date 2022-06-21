#include "StdAfx.h"
#include "GSClientMgr.h"
#include "GSServer.h"
#include "ConsoleHelper.h"
#include <boost/make_shared.hpp>
#include "GSAllocator.h"

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

	int debug_null_cnt = 0;
	int connection_cnt = 0;

	for each (auto& client in m_Clients)
	{
		if (client.second == NULL)
		{
			debug_null_cnt++;
			continue;
		}

		if (client.second->GetConnected())
		{
			DWORD client_time = client.second->GetAliveTime();
			DWORD server_check_time = pServer->GetArgument().m_AliveTime;
			DWORD system_tick = GetTickCount();
			int Diff = system_tick - (client_time + server_check_time);

			if (client_time != 0 && (client_time + server_check_time) <= system_tick)
			{
				pServer->Close(client.second->GetTCPSocket()->GetSocket());
			}

			connection_cnt++;
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
					SYSLOG().Write("DelClient is not 0 failed \n");
				}
			}
			else
				m_ReInsert_Queue.push(client);

			if ((ClientTime < SYSTime) && count != 0)
			{
				SYSLOG().Write("count is not 0 %d \n", count);
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

	//접속가능한 세션이 50명 이하면 다시 100개 할당한다. 
	if ((m_MaxClients - connection_cnt) < 50)
		NewClient(false);

	//SYSTEMTIME		sysTime;
	//::GetLocalTime(&sysTime);

	//SYSLOG().Write("[ %04d-%02d-%02d %02d:%02d:%02d ] Current User Count %d connectable count %d Debug Count %d\n",
	//	sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, pServer->CurrentPlayerCount(), ConnectableSocketCount(), DebugCount.fetch_add(0));

//	SYSLOG().Write("\nre m_insert_queue queue %d\n", m_ReInsert_Queue.unsafe_size());
//	SYSLOG().Write("re m_remove_queue queue %d\n", m_Remove_Queue.unsafe_size());
//	SYSLOG().Write("\nconnect socket count %d GetActiveSocketCount %d ConnectableSocketCount %d\n", 
//	pServer->CurrentPlayerCount(), GetActiveSocketCount(), ConnectableSocketCount());

	//SYSLOG().Write("[UserCount : %d] [Debug Count : %d]\n",
	//	pServer->CurrentPlayerCount(), DebugCount.fetch_add(0));
	{

		char msg[256];
		
		sprintf_s(msg, 256, "[Conncted:%d] [Debug:%d] [RemoveQ:%zd] [accept :%d] [Toal New : %d][Total Close : %d] [debug_null %d]\n",
			connection_cnt, DebugCount.fetch_add(0), m_Remove_Queue.unsafe_size(), ConnectCount.fetch_add(0), NewConnectount.fetch_add(0), DisConnectCount.fetch_add(0), debug_null_cnt);
		//	pServer->TotalRecvBytes.fetch_add(0)/1024/1024,pServer->TotalSendBytes.fetch_add(0) / 1024 / 1024);

		ConsoleHelper::DebugConsoleString(0, msg);

	}
}

int GSClientMgr::GetActiveSocketCount()
{
	int count = 0;

	for each (auto& client in m_Clients)
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
	auto& find = m_Clients.find(id);
	if (find == m_Clients.end())
		return NULL;

	return m_Clients[id];

}

BOOL GSClientMgr::AddClient(GSCLIENT_PTR newclient)
{
	auto& find = m_Clients.find(newclient->GetId());
	if (find != m_Clients.end())
	{
		if (find->second != NULL)
			printf("alert add client id is exist\n");
	}

	m_Clients[newclient->GetId()] = newclient;

	newclient->m_GSServer = this->m_GSServer;


	return TRUE;
}

BOOL GSClientMgr::BookDelClient(int id)
{
	auto& client = GetClient(id);

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

	return TRUE;
}

int GSClientMgr::PopRecycleId()
{
	int id;
	if (m_Reycle_Id_Queue.try_pop(id) == true)
		return id;

	return -1;
}

void GSClientMgr::InsertRecycleId(int _id)
{
	m_Reycle_Id_Queue.push(_id);
}

BOOL GSClientMgr::NewClient(bool disalloc)
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;
	SOCKET ListenSocket = pServer->GetTcpListen()->GetSocket();

	if (!ListenSocket)
	{
		SYSLOG().Write("NewClient !ListenSocket \n");
		return FALSE;
	}

	CThreadSync Sync;

	int NewClient = 1;
	//임시 주석
	if ((m_MaxClients - std::abs(ConnectCount - DisConnectCount)) < 50)
	{
		if(disalloc == true)
			NewClient = 101;
		else
			NewClient = 100;

		SYSLOG().Write("Resize Client  %d \n", NewClient);

		m_MaxClients += NewClient;
	}
	
	NewConnectount.fetch_add(NewClient);


	for (int i = 0; i < NewClient; i++)
	{
		GSCLIENT_PTR pClient = ALLOCATOR.Create<GSClient>();

		int newid = PopRecycleId();
		if (newid == -1)
			newid = IncClientId();

		pClient->SetId(newid);
		pClient->Create(TCP);
		pClient->m_GSServer = pServer;
		pClient->GetTCPSocket()->m_UseCompress = pServer->GetArgument().m_UseCompress;
		if (AddClient(pClient) == FALSE)
		{
			SYSLOG().Write("NewClient failed...1 \n");
		}


		if (!pClient->GetTCPSocket()->Initialize())
		{
			SYSLOG().Write("NewClient failed...2 \n");
			DelClient(pClient->GetId());
			End();
			return FALSE;
		}

		if (!pClient->GetTCPSocket()->Accept(ListenSocket))
		{
			SYSLOG().Write("NewClient failed...3 \n");
			DelClient(pClient->GetId());
			End();
			return FALSE;
		}
	}

	return TRUE;
}


GSCLIENT_PTR GSClientMgr::NewClient2()
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;
	SOCKET ListenSocket = pServer->GetTcpListen()->GetSocket();

	if (!ListenSocket)
	{
		SYSLOG().Write("NewClient !ListenSocket \n");
		return FALSE;
	}

	CThreadSync Sync;
	for (int i = 0; i < 1; i++)
	{
		GSCLIENT_PTR pClient = ALLOCATOR.Create<GSClient>();

		int newid = PopRecycleId();
		if (newid == -1)
			newid = IncClientId();

		pClient->SetId(newid);
		pClient->Create(TCP,ClientType::SERVER_CLIENT);
		pClient->m_GSServer = pServer;

		if (AddClient(pClient) == FALSE)
		{
			SYSLOG().Write("NewClient failed...1 \n");
		}

		return pClient;
	}

	return NULL;
}


BOOL GSClientMgr::Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID pServer)
{
	if (!ListenSocket)
		return FALSE;

	DWORD MaxUser = m_MaxClients = MaxClients;


	GSServer::GSServer* Server = (GSServer::GSServer*)pServer;


	for (DWORD i=0;i<MaxUser;i++)
	{
		GSCLIENT_PTR pClient = ALLOCATOR.Create<GSClient>();
		pClient->SetId(IncClientId());
		pClient->Create(TCP);
		pClient->m_GSServer = pServer;
		pClient->GetTCPSocket()->m_UseCompress = Server->GetArgument().m_UseCompress;

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

		pClient->m_Socket = pClient->GetTCPSocket()->GetSocket();

		AddClient(pClient);
	}

	return TRUE;
}

VOID GSClientMgr::End()
{

}

}	}