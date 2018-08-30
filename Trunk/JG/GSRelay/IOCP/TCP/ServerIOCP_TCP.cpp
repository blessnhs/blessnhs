#include "StdAfx.h"
#include "ServerIocp_TCP.h"
#include "TCPSession.h"
#include "../../GSIniContext.h"

namespace GSRelayServer {	namespace TCPServerIOCP	{


CServerIOCP_TCP::CServerIOCP_TCP(VOID)
{
	m_ConnectedSessionManager = new TCPSessionMgr;
}

CServerIOCP_TCP::~CServerIOCP_TCP(VOID)
{
}

VOID CServerIOCP_TCP::OnConnected(VOID *pObject)
{
	TCP_SESSION *pConnectedSession = reinterpret_cast<TCP_SESSION*>(pObject);
	
	if (!GSIocp::RegIocpHandler(pConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(pConnectedSession))) 
		return;

	if (!pConnectedSession->InitializeReadForIocp())
	{ 
		OnDisconnected(pConnectedSession);
		return; 
	}

 	pConnectedSession->OnConnect();

	pConnectedSession->SetConnected(TRUE);
}

GSPacketTCP		*CServerIOCP_TCP::GetListen()
{
	return m_pListen;
}

VOID CServerIOCP_TCP::OnDisconnected(VOID *pObject)
{
	TCP_SESSION *pConnectedSession = reinterpret_cast<TCP_SESSION*>(pObject);

	pConnectedSession->OnDisconnect();
}

VOID CServerIOCP_TCP::OnRead(VOID *pObject, DWORD dwDataLength)
{
	TCP_SESSION *pConnectedSession = reinterpret_cast<TCP_SESSION*>(pObject);

	pConnectedSession->OnRecv(dwDataLength);

	if (!pConnectedSession->InitializeReadForIocp())
		OnDisconnected(pConnectedSession);
}

TCPSessionMgr* CServerIOCP_TCP::GetSessionMgr()
{
	return m_ConnectedSessionManager;
}

VOID CServerIOCP_TCP::OnWrote(VOID *pObject, DWORD dwDataLength)
{
	TCP_SESSION *pConnectedSession = reinterpret_cast<TCP_SESSION*>(pObject);
	pConnectedSession->WriteComplete();
}

BOOL CServerIOCP_TCP::Begin(VOID)
{
	if (!GSIocp::Initialize()) return FALSE;

	m_pListen = new GSPacketTCP();

	if (!m_pListen->Initialize())
	{
		CServerIOCP_TCP::End();

		return FALSE;
	}

	if (!m_pListen->CreateTcpSocket())
	{
		CServerIOCP_TCP::End();

		return FALSE;
	}
	


	if (!m_pListen->Listen((WORD)INICONTEXT.m_Port, INICONTEXT.m_MaxPlayerCount))
	{
		CServerIOCP_TCP::End();

		return FALSE;
	}

	if (!GSIocp::RegIocpHandler(m_pListen->GetSocket(), reinterpret_cast<ULONG_PTR>(m_pListen)))
	{
		CServerIOCP_TCP::End();

		return FALSE;
	}

	if (!m_ConnectedSessionManager->Begin(m_pListen->GetSocket()))
	{
		CServerIOCP_TCP::End();

		return FALSE;
	}

	return TRUE;
}

VOID CServerIOCP_TCP::End(VOID)
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

	m_ConnectedSessionManager->End();

	if (m_pListen)
	{
		m_pListen->Termination();

		delete m_pListen;
	}
}

extern CServerIOCP_TCP &GetServerIOCP_TCP()
{
	static CServerIOCP_TCP g_IOCP;
	return g_IOCP;
}

}	}