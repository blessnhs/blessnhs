#include "StdAfx.h"
#include "GSServer.h"
#include "GSClient.h"
#include "GSSocketUDP.h"
#include "Systimer.h"
#include "MiniDump.h"
#include "GSMainProcess.h"
#include <boost/make_shared.hpp>

namespace GSNetwork	{	namespace GSServer	{

GSServer::GSServer(void)
{
	CoInitialize(NULL);

	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);	

	if (!GSMiniDump::GSMiniDump::Begin())
		return ;

	InitializeCriticalSection(&m_PublicLock);
}

GSServer::~GSServer(void)
{
	DeleteCriticalSection(&m_PublicLock);

	WSACleanup();
	GSNetwork::GSMiniDump::GSMiniDump::End();
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
		return;

	pClient->OnRecv(dataLength,pClient);

	if (!pClient->InitializeReadForIocp())
		OnDisconnected(client_id);
}

VOID GSServer::OnWrote(int client_id, DWORD dataLength)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
		return;

	pClient->WriteComplete();
}

VOID GSServer::OnConnected(int client_id)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
		return;

	if (!GSIocp::RegIocpHandler(pClient->GetSocket(), reinterpret_cast<ULONG_PTR>(&pClient))) 
		return;

	if (!pClient->InitializeReadForIocp())
	{ 
		pClient->Recycle(m_pTCPListen->GetSocket()); 
		return; 
	}

	pClient->OnConnect();	
}

VOID GSServer::OnDisconnected(int client_id)
{
	boost::shared_ptr<GSClient> pClient = GetClient(client_id);
	if (pClient == NULL)
		return;

	pClient->OnDisconnect(pClient);
}

GSCLIENT_PTR GSServer::GetTcpListen()
{
	return m_pTCPListen;
}

std::vector<GSCLIENT*>&	GSServer::GetUDPListenPorts()
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

	for(int i=0;i<m_UDPListenPorts.size();i++)
	{
		GSCLIENT *Socket = m_UDPListenPorts[i];

		if(Socket != NULL)
			delete Socket;
	}

	m_UDPListenPorts.clear();

	for(int i=0;i<m_FreePorts.size();i++)
	{
		GSSocket::GSSocketUDP::GSSocketUDP *Socket = m_FreePorts[i];

		if(Socket != NULL)
			delete Socket;
	}

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
		m_UDPListenPorts[i] = new GSCLIENT;
		m_UDPListenPorts[i]->Create(UDP);
	
		if (!m_UDPListenPorts[i]->GetUDPSocket()->Initialize())
		{
			GSServer::End();

			return FALSE;
		}

		if (!m_UDPListenPorts[i]->GetUDPSocket()->CreateUdpSocket(m_Arguments.m_UdpPorts[i]))
		{
			GSServer::End();

			return FALSE;
		}

		if (!GSIocp::RegIocpHandler(m_UDPListenPorts[i]->GetSocket(), reinterpret_cast<ULONG_PTR>(m_UDPListenPorts[i])))
		{
			GSServer::End();

			return FALSE;
		}

		m_FreePorts.push_back(m_UDPListenPorts[i]->GetUDPSocket().get());
		m_UDPListenPorts[i]->GetUDPSocket()->InitializeReadFromForIocp();
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
	
	auto client = m_ClientMgr.m_Clients.find(_Id);

	if (client == m_ClientMgr.m_Clients.end())
	{
		return NULL;
	}

	return client->second;
}

int GSServer::IncClinetId()
{
	static atomic<int> intAtomic = 0;

	intAtomic.fetch_add(1);       

	return intAtomic;
}

}	}