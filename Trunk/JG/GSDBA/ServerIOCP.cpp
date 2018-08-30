#include "StdAfx.h"
#include "stdafx.h"
#include "ServerIocp.h"

namespace GSDBA	{	namespace	CServerIOCP	{

CServerIOCP::CServerIOCP(VOID)
{
}

CServerIOCP::~CServerIOCP(VOID)
{
}

VOID CServerIOCP::KeepThreadCallback(VOID)
{
	// KeepAlive 신호
	DWORD dwKeepAlive = 0xFFFF;

	while (TRUE)
	{
		// 30초동안 종료 이벤트가 발생하지 않으면 넘어갑니다.
		DWORD dwResult = WaitForSingleObject(m_hKeepThreadDestroyEvent, 30000);

		// 종료 이벤트가 발생했을경우 Thread를 종료합니다.
		if (dwResult == WAIT_OBJECT_0) return;

		// 접속해 있는 모든 Session에 패킷을 전송합니다.
		m_oConnectedSessionManager.WriteAll(0x3000000, (BYTE*)&dwKeepAlive, sizeof(DWORD));
	}
}

// CIocp의 가상함수들
VOID CServerIOCP::OnConnected(VOID *pObject)
{
	// 여기 가상함수에서 넘어온 pObject는 접속을 담당한 개체가 넘어오게 됩니다.
	// 이것을 여기서 클라이언트를 관리할 CConnectedSession으로 형변환을 해 주어 받게 됩니다.
	// 형변환은 reinterpret_cast를 사용합니다.
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);

	printf("OnConncted\n");
	
	// 접속한 개체의 IO를 IOCP를 통해서 받기위해서 IOCP에 등록하는 과정을 거치게 됩니다.
	// 여기서 GetSocket을 이용해서 소켓을 등록하고 해당 키는 개체의 포인터를 이용하게 됩니다.
	if (!GSIocp::RegIocpHandler(pConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(pConnectedSession))) 
		return;

	// IOCP 초기 받기를 실행해 줍니다.
	if (!pConnectedSession->InitializeReadForIocp())
	{ 
		// 만일 실패했을 경우 개체를 재시작해 줍니다.
		pConnectedSession->Recycle(m_pListen->GetSocket()); 
		return; 
	}

	pConnectedSession->SetConnected(TRUE);
}

VOID CServerIOCP::OnDisconnected(VOID *pObject)
{
	// 여기 가상함수에서 넘어온 pObject는 OnConnected와 마찬가지로 접속을 담당한 개체가 넘어오게 됩니다.
	// 이것을 여기서 클라이언트를 관리할 CConnectedSession으로 형변환을 해 주어 받게 됩니다.
	// 형변환은 reinterpret_cast를 사용합니다.
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);

	if(pConnectedSession->GetConnected() == FALSE) return ;

	printf("OnDisConncted\n");

	// 접속을 종료하였기 때문에 개체를 재시작해 줍니다.
	pConnectedSession->Recycle(m_pListen->GetSocket());

	pConnectedSession->SetConnected(FALSE);
}

VOID CServerIOCP::OnRead(VOID *pObject, DWORD dwDataLength)
{
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);

	WORD MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;
	BYTE Packet[MAX_BUFFER_LENGTH] = {0,};

	if (pConnectedSession->ReadPacket(dwDataLength))
	{
		while (pConnectedSession->GetPacket(MainProtocol, SubProtocol,Packet, dwPacketLength))
		{
			pConnectedSession->Process(Packet,dwPacketLength,MainProtocol);
		}
	}
	else 
		OnDisconnected(pConnectedSession);

	if (!pConnectedSession->InitializeReadForIocp())
		OnDisconnected(pConnectedSession);
}

VOID CServerIOCP::OnWrote(VOID *pObject, DWORD dwDataLength)
{
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);
	pConnectedSession->WriteComplete();
}

BOOL CServerIOCP::Begin(VOID)
{
	if (!GSIocp::Initialize()) return FALSE;

	m_pListen = new GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP();

	if (!m_pListen->Initialize())
	{
		CServerIOCP::End();

		return FALSE;
	}

	if (!m_pListen->CreateTcpSocket())
	{
		CServerIOCP::End();

		return FALSE;
	}

	if (!m_pListen->Listen(8593, MAX_USER))
	{
		CServerIOCP::End();

		return FALSE;
	}

	if (!GSIocp::RegIocpHandler(m_pListen->GetSocket(), reinterpret_cast<ULONG_PTR>(m_pListen)))
	{
		CServerIOCP::End();

		return FALSE;
	}

	if (!m_oConnectedSessionManager.Begin(m_pListen->GetSocket()))
	{
		CServerIOCP::End();

		return FALSE;
	}

	return TRUE;
}

VOID CServerIOCP::End(VOID)
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

	m_oConnectedSessionManager.End();

	if (m_pListen)
	{
		m_pListen->Termination();

		delete m_pListen;
	}
}

}	}