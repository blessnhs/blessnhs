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

VOID GSClientMgr::AllCloseSocket()
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;
	for each (auto & client in m_Clients)
	{
		if (client.second == NULL)
		{
			continue;
		}

		if (client.second->GetConnected())
		{
				pServer->Close(client.second->GetTCPSocket()->GetSocket());
		}
	}
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

			if (server_check_time != 0 && client_time != 0 && (client_time + server_check_time) <= system_tick)
			{
				if (client.second->GetClientType() != ClientType::SERVER_CLIENT)
				{
					client.second->m_TryCloseCount++;

					
					if(client.second->m_TryCloseCount > 1)
					{
						//이 코드는 사실상 들오면 안된다.
						//client.second->OnDisconnect(client.second, true);
					}
					else if(client.second->m_TryCloseCount == 1)
					{
						//여기서 서버에서 그냥 close하면 무한 무프 ondisconnect-> close 반복이라 이런식으로 iocp에서 감지하겠끔 해준다.
						byte data[2];
						client.second->GetTCPSocket()->Write(data, 1);
//						pServer->Close(client.second->GetTCPSocket()->GetSocket());
					}
				}
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

//	SYSTEMTIME		sysTime;
//	::GetLocalTime(&sysTime);

//	SYSLOG().Write("[ %04d-%02d-%02d %02d:%02d:%02d ] Current User Count %d connectable count %d Debug Count %d\n",
//		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, pServer->CurrentPlayerCount(), ConnectableSocketCount(), DebugCount.fetch_add(0));

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
	//만번부터 
	if (m_Reycle_Id_Queue.unsafe_size() < 10000)
		return -1;

	int id;
	if (m_Reycle_Id_Queue.try_pop(id) == true)
		return id;

	return -1;
}

void GSClientMgr::InsertRecycleId(int _id)
{
	m_Reycle_Id_Queue.push(_id);
}


//모든 문제의 발단
//listen(backlog)수만큼 접속가능하고 거기서 감소 (iocp에서 떨어지는disconnect만큼 acceptex를 해주면딘다.)
//임의로 콜한 disconnect에서 잘못해줬다가는 time_wait close_wait같은 tcp 4hand shake상에서 close_wait이 남게된다.)
//접속 및 종료는 딱딱 맞춰줘야 한다.

BOOL GSClientMgr::NewClient(int NewClient)
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;
	SOCKET ListenSocket = pServer->GetTcpListen()->GetSocket();

	if (!ListenSocket)
	{
		SYSLOG().Write("NewClient !ListenSocket \n");
		return FALSE;
	}

	CThreadSync Sync;
			
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