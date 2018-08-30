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
	// KeepAlive ��ȣ
	DWORD dwKeepAlive = 0xFFFF;

	while (TRUE)
	{
		// 30�ʵ��� ���� �̺�Ʈ�� �߻����� ������ �Ѿ�ϴ�.
		DWORD dwResult = WaitForSingleObject(m_hKeepThreadDestroyEvent, 30000);

		// ���� �̺�Ʈ�� �߻�������� Thread�� �����մϴ�.
		if (dwResult == WAIT_OBJECT_0) return;

		// ������ �ִ� ��� Session�� ��Ŷ�� �����մϴ�.
		m_oConnectedSessionManager.WriteAll(0x3000000, (BYTE*)&dwKeepAlive, sizeof(DWORD));
	}
}

// CIocp�� �����Լ���
VOID CServerIOCP::OnConnected(VOID *pObject)
{
	// ���� �����Լ����� �Ѿ�� pObject�� ������ ����� ��ü�� �Ѿ���� �˴ϴ�.
	// �̰��� ���⼭ Ŭ���̾�Ʈ�� ������ CConnectedSession���� ����ȯ�� �� �־� �ް� �˴ϴ�.
	// ����ȯ�� reinterpret_cast�� ����մϴ�.
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);

	printf("OnConncted\n");
	
	// ������ ��ü�� IO�� IOCP�� ���ؼ� �ޱ����ؼ� IOCP�� ����ϴ� ������ ��ġ�� �˴ϴ�.
	// ���⼭ GetSocket�� �̿��ؼ� ������ ����ϰ� �ش� Ű�� ��ü�� �����͸� �̿��ϰ� �˴ϴ�.
	if (!GSIocp::RegIocpHandler(pConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(pConnectedSession))) 
		return;

	// IOCP �ʱ� �ޱ⸦ ������ �ݴϴ�.
	if (!pConnectedSession->InitializeReadForIocp())
	{ 
		// ���� �������� ��� ��ü�� ������� �ݴϴ�.
		pConnectedSession->Recycle(m_pListen->GetSocket()); 
		return; 
	}

	pConnectedSession->SetConnected(TRUE);
}

VOID CServerIOCP::OnDisconnected(VOID *pObject)
{
	// ���� �����Լ����� �Ѿ�� pObject�� OnConnected�� ���������� ������ ����� ��ü�� �Ѿ���� �˴ϴ�.
	// �̰��� ���⼭ Ŭ���̾�Ʈ�� ������ CConnectedSession���� ����ȯ�� �� �־� �ް� �˴ϴ�.
	// ����ȯ�� reinterpret_cast�� ����մϴ�.
	CNetworkSession *pConnectedSession = reinterpret_cast<CNetworkSession*>(pObject);

	if(pConnectedSession->GetConnected() == FALSE) return ;

	printf("OnDisConncted\n");

	// ������ �����Ͽ��� ������ ��ü�� ������� �ݴϴ�.
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