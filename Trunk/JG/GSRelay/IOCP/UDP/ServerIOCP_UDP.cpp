#include "stdafx.h"
#include "ServerIocp_UDP.h"
#include "../../GSIniContext.h"
#include "../GSPacket/Relay.h"


namespace GSRelayServer {	namespace UDPServerIOCP	{

	class GSRelayServer::UDPNetworkSession::CNetworkSession;

CServerIOCP_UDP::CServerIOCP_UDP(VOID)
{
}

CServerIOCP_UDP::~CServerIOCP_UDP(VOID)
{
}

VOID CServerIOCP_UDP::OnConnected(VOID *pObject)
{
	GSRelayServer::UDPNetworkSession::CNetworkSession *pConnectedSession = reinterpret_cast<GSRelayServer::UDPNetworkSession::CNetworkSession*>(pObject);
	
	if (!GSIocp::RegIocpHandler(pConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(pConnectedSession))) 
		return;

	if (!pConnectedSession->InitializeReadFromForIocp())
	{ 
		OnDisconnected(pConnectedSession);
		return; 
	}

 	pConnectedSession->OnConnect();
}

UDPNetworkSession::CNetworkSession		*CServerIOCP_UDP::GetListen(int pos)
{
	return m_pListen[pos];
}

VOID CServerIOCP_UDP::OnDisconnected(VOID *pObject)
{
	GSRelayServer::UDPNetworkSession::CNetworkSession *pConnectedSession = reinterpret_cast<GSRelayServer::UDPNetworkSession::CNetworkSession*>(pObject);

	pConnectedSession->OnDisconnect();
}

VOID CServerIOCP_UDP::OnRead(VOID *pObject, DWORD dwDataLength)
{
	GSRelayServer::UDPNetworkSession::CNetworkSession *pConnectedSession = reinterpret_cast<GSRelayServer::UDPNetworkSession::CNetworkSession*>(pObject);

	pConnectedSession->OnRecv(dwDataLength);

	if (!pConnectedSession->InitializeReadFromForIocp())
		OnDisconnected(pConnectedSession);
}

VOID CServerIOCP_UDP::OnWrote(VOID *pObject, DWORD dwDataLength)
{
	GSRelayServer::UDPNetworkSession::CNetworkSession *pConnectedSession = reinterpret_cast<GSRelayServer::UDPNetworkSession::CNetworkSession*>(pObject);
	pConnectedSession->WriteComplete();
}

BOOL CServerIOCP_UDP::Begin(VOID)
{
	if (!GSIocp::Initialize()) return FALSE;

	int MaxPort = INICONTEXT.m_UdpCount;

	for(int i=0;i<MaxPort;i++)
	{
		m_pListen[i] = new UDPNetworkSession::CNetworkSession();
		m_pListen[i]->id = i+MaxPort;
	
		if (!m_pListen[i]->Initialize())
		{
			CServerIOCP_UDP::End();

			return FALSE;
		}

		if (!m_pListen[i]->CreateUdpSocket(INICONTEXT.m_UdpPort+i))
		{
			CServerIOCP_UDP::End();

			return FALSE;
		}

		if (!GSIocp::RegIocpHandler(m_pListen[i]->GetSocket(), reinterpret_cast<ULONG_PTR>(m_pListen[i])))
		{
			CServerIOCP_UDP::End();

			return FALSE;
		}

		m_FreePorts.push_back(INICONTEXT.m_UdpPort+i);
		m_pListen[i]->InitializeReadFromForIocp();
	}

	return TRUE;
}

VOID CServerIOCP_UDP::End(VOID)
{

	if (m_hKeepThread)
	{
		SetEvent(m_hKeepThreadDestroyEvent);

		WaitForSingleObject(m_hKeepThread, INFINITE);

	}

	if (m_hKeepThreadDestroyEvent)
	{
		CloseHandle(m_hKeepThreadDestroyEvent);
		m_hKeepThreadDestroyEvent = NULL;
	}

	GSIocp::Termination();

	if (m_pListen)
	{
		for(int i=0;i<2000;i++)
		m_pListen[i]->Termination();

		delete m_pListen;
	}
}

extern CServerIOCP_UDP &GetServerIOCP_UDP()
{
	static CServerIOCP_UDP g_IOCP;
	return g_IOCP;
}

}	}