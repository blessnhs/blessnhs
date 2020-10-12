#include "StdAfx.h"
#include "GSServer.h"
#include "GSClient.h"
#include "GSSocketUDP.h"
#include "Systimer.h"
#include "MiniDump.h"
#include "GSMainProcess.h"
#include <boost/make_shared.hpp>

atomic<int>		ConnectCount = 0;
atomic<int>		DisConnectCount = 0;

namespace GSNetwork	{	namespace GSServer	{
	

GSServer::GSServer(void)
{
	CoInitialize(NULL);

	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);	

	m_ClientMgr.SetGSServer(this);
}

GSServer::~GSServer(void)
{
	WSACleanup();
	CoUninitialize();
}

bool GSServer::Create(GSArgument arg)
{
	m_Arguments = arg;

	MAINPROC.Create(1);

	if(m_Arguments.m_LogicThreadCnt >= MSG_TYPE_DB_1)
		m_Arguments.m_LogicThreadCnt = MSG_TYPE_DB_1 - 1;

	for(int i=0;i<m_Arguments.m_LogicThreadCnt;i++)
		MAINPROC.Activate(MSG_TYPE_USER+i,1);

	for(int i=0;i<m_Arguments.m_DBThreadCnt;i++)
		MAINPROC.Activate(MSG_TYPE_DB_1+i   ,1);

	MAINPROC.Activate(MSG_TYPE_LOG   ,1);
	
	BeginTCP();
	BeginUDP();

	GetSysTimer().Start((LPVOID)this);

	return true;
}


VOID GSServer::OnRead(int client_id, DWORD dataLength)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
	{
		return;
	}

	pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_REC.fetch_sub(1);

	pClient->OnRecv(dataLength,pClient);

	if (!pClient->InitializeReadForIocp())
	{
		this->AddPlayerCount(1);
		OnDisconnected(client_id);	//add 	
	}

}

VOID GSServer::OnWrote(int client_id, DWORD dataLength)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
	{
		return;
	}

	pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_SND.fetch_sub(1);

	pClient->WriteComplete();
}

VOID GSServer::OnConnected(int client_id)
{
	CThreadSync Sync;

	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
	{
		printf("!!!alert cant find onconnect client id\n");
		return;
	}
	static int total_cnt = 0;

	ConnectCount.fetch_add(1);

	pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_ACC.fetch_sub(1);

	m_EvtClientId = client_id;

	if (!GSIocp::RegIocpHandler(pClient->GetSocket(), reinterpret_cast<ULONG_PTR>(&m_EvtClientId)))
	{
		printf("Connected exception ...1\n");
		//이코드에서 그냥 close하면 gsclient는 서버에 접속도 못하는 미아 객체가 됨
		this->AddPlayerCount(1);
		OnDisconnected(client_id, true);
		//pClient->Close();
		return;
	}


	if (!pClient->InitializeReadForIocp())
	{ 
		printf("Connected exception ...2\n");
		//이코드에서 그냥 close하면 gsclient는 서버에 접속도 못하는 미아 객체가 됨
		this->AddPlayerCount(1);
		OnDisconnected(client_id,true);
		//pClient->Close();
		return; 
	}


	pClient->OnConnect(pClient);
}

VOID GSServer::OnDisconnected(int client_id, bool isForce)
{
	CThreadSync Sync;
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
	{
		printf("!!!alert cant find OnDisconnected client id\n");
		return;
	}

	DisConnectCount.fetch_add(1);

	pClient->OnDisconnect(pClient, isForce);

}

VOID GSServer::OnDisconnected2(int client_id, int type)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
	{
		printf("!!!alert cant find OnDisconnected2222 client id\n");
		return;
	}

	if (type == IO_TYPE::IO_ACCEPT)
	{
		pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_ACC.fetch_sub(1);
	}
	else if (type == IO_TYPE::IO_READ)
	{
		pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_REC.fetch_sub(1);
	}
	else if (type == IO_TYPE::IO_WRITE)
	{
		pClient->GetTCPSocket()->m_OLP_REMAIN_COUNT_SND.fetch_sub(1);
	}

	OnDisconnected(client_id);
}

GSCLIENT_PTR GSServer::GetTcpListen()
{
	return m_pTCPListen;
}

concurrency::concurrent_unordered_map<DWORD, GSCLIENT_PTR>&	GSServer::GetUDPListenPorts()
{
	return m_UDPListenPorts;
}

GSClientMgr::GSClientMgr&	GSServer::GetClientMgr()
{
	return m_ClientMgr;
}

VOID GSServer::End(VOID)
{
	GSIocp::Termination();

	m_ClientMgr.End();

	if (m_pTCPListen)
	{
		m_pTCPListen->GetTCPSocket()->Termination();

	}

	m_UDPListenPorts.clear();

	m_FreePorts.clear();

}

BOOL GSServer::BeginTCP()
{
	if (!GSIocp::Initialize()) return FALSE;

	m_pTCPListen = boost::make_shared<GSClient>();
	m_pTCPListen->Create(TCP);

	if (!m_pTCPListen->GetTCPSocket()->Initialize())
	{
		GSServer::End();

		return FALSE;
	}

	if (!m_pTCPListen->GetTCPSocket()->CreateTcpSocket())
	{
		GSServer::End();

		return FALSE;
	}

	if (!m_pTCPListen->GetTCPSocket()->Listen((WORD)m_Arguments.m_BindPort, m_Arguments.m_MaxClient))
	{
		GSServer::End();

		return FALSE;
	}

	if (!GSIocp::RegIocpHandler(m_pTCPListen->GetSocket(), (ULONG_PTR)((&m_pTCPListen))))
	{
		GSServer::End();

		return FALSE;
	}

	if (!m_ClientMgr.Begin(m_pTCPListen->GetSocket(),m_Arguments.m_MaxClient,this))
	{
		GSServer::End();

		return FALSE;
	}

	return TRUE;
}

BOOL GSServer::BeginUDP()
{
//	if (!GSIocp::Initialize()) return FALSE;

	int MaxPort = m_Arguments.m_UdpPorts.size();

	for(int i=0;i<MaxPort;i++)
	{
		auto UDPListenPort = boost::make_shared<GSCLIENT>();
		UDPListenPort->SetId(IncClinetId());

		UDPListenPort->SetId(IncClinetId());
		UDPListenPort->Create(UDP);
	
		if (!UDPListenPort->GetUDPSocket()->Initialize())
		{
			GSServer::End();

			return FALSE;
		}

		if (!UDPListenPort->GetUDPSocket()->CreateUdpSocket(m_Arguments.m_UdpPorts[i]))
		{
			GSServer::End();

			return FALSE;
		}

		if (!GSIocp::RegIocpHandler(UDPListenPort->GetSocket(), reinterpret_cast<ULONG_PTR>(UDPListenPort->GetUDPSocket().get())))
		{
			GSServer::End();

			return FALSE;
		}

		UDPListenPort->GetUDPSocket()->InitializeReadFromForIocp();

		m_FreePorts.push(UDPListenPort->GetUDPSocket());
		m_UDPListenPorts[UDPListenPort->GetId()] = UDPListenPort;

	}

	return TRUE;
}

GSArgument &GSServer::GetArgument()
{
	return m_Arguments;
}

GSCLIENT_PTR	GSServer::GetClient(DWORD _Id)
{
	if(_Id < 0 ) return NULL;
	
	auto client = m_ClientMgr.GetClient(_Id);

	return client;
}

int GSServer::IncClinetId()
{
	static atomic<int> intAtomic = 0;

	intAtomic.fetch_add(1);       

	return intAtomic;
}

}	}