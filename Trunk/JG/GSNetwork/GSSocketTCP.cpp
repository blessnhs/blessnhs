#include "StdAfx.h"
#include "GSSocketTCP.h"
#include "zip.h"
#include "lzf.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketTCP	{

GSSocketTCP::GSSocketTCP(void)
{
}

GSSocketTCP::~GSSocketTCP(void)
{
	
}


BOOL GSSocketTCP::Initialize(VOID)
{
	return TRUE;
}

BOOL GSSocketTCP::Termination(VOID)
{
	return GSSocket::Termination();
}

BOOL GSSocketTCP::GetPeerInfo(LPSTR szAddr, unsigned short &usPort)
{
	struct sockaddr_in addr;
	int addrsize = sizeof(sockaddr_in);
	memset((void *)&addr, 0, addrsize);
	int nResult = getpeername(m_Socket, (sockaddr*)&addr, &addrsize);
	if (nResult == SOCKET_ERROR)
	{
		auto error = GetLastError();
		return false;
	}
	if(szAddr != NULL) {
		strcpy((char*)szAddr, inet_ntoa(addr.sin_addr));
		usPort = ntohs(addr.sin_port);
	}
	return TRUE;
}

BOOL GSSocketTCP::CreateTcpSocket(VOID)
{
	CThreadSync Sync;

	if (m_Socket)
		return FALSE;

	m_Socket	= WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (m_Socket == INVALID_SOCKET)
		return FALSE;

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));
	return TRUE;
}

BOOL GSSocketTCP::InitializeReadForIocp(VOID)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	WSABUF	WsaBuf;
	DWORD	ReadBytes	= 0;
	DWORD	ReadFlag	= 0;

	WsaBuf.buf			= (CHAR*) m_Buffer;
	WsaBuf.len			= MAX_BUFFER_LENGTH;

	m_OLP_REMAIN_COUNT_REC.fetch_add(1);

	m_Read_OLP->ObjectId = m_ClientId;

	INT		ReturnValue = WSARecv(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		&m_Read_OLP->Overlapped,
		NULL);


	if (ReadBytes > MAX_BUFFER_LENGTH)
		SYSLOG().Write("wsa recv size over!!\n");

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_OLP_REMAIN_COUNT_REC.fetch_sub(1);
		Termination();

		return FALSE;
	}

	return TRUE;
}

BOOL			GSSocketTCP::ReadForIocp(DWORD dataLength,DWORD RemainLength,DWORD MaxPacketBufferSize)
{
	//m_Buffer �̹��� ���� ����
	//m_PacketBuffer ���� ����


	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!PacketBuffer || dataLength <= 0)
		return FALSE;

	if(RemainLength + dataLength >= MaxPacketBufferSize)
		return FALSE;

	memcpy(PacketBuffer + RemainLength, m_Buffer, dataLength);

	return TRUE;
}

BOOL			GSSocketTCP::ReadForEventSelect(BYTE *data, DWORD &dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!data)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	WSABUF	WsaBuf;
	DWORD	ReadBytes	= 0;
	DWORD	ReadFlag	= 0;

	WsaBuf.buf			= (CHAR*) m_Buffer;
	WsaBuf.len			= MAX_BUFFER_LENGTH;

	m_OLP_REMAIN_COUNT_REC.fetch_add(1);

	m_Read_OLP->ObjectId = m_ClientId;

	INT		ReturnValue = WSARecv(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		&m_Read_OLP->Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_OLP_REMAIN_COUNT_REC.fetch_sub(1);
		Termination();

		return FALSE;
	}

	memcpy(data, m_Buffer, ReadBytes);
	dataLength = ReadBytes;

	return TRUE;

}

VOID GSSocketTCP::SetConnected(BOOL bConnected) 
{
	CThreadSync Sync; 
	m_bConnected = bConnected; 
}
BOOL GSSocketTCP::GetConnected(VOID) 
{ 
	CThreadSync Sync; 
	return m_bConnected; 
}

BOOL GSSocketTCP::Write(BYTE *data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!data || dataLength <=0)
		return FALSE;
	

	WSABUF	WsaBuf;
	DWORD	WriteBytes	= 0;
	DWORD	WriteFlag	= 0;

	WsaBuf.buf			= (CHAR*) data;
	WsaBuf.len			= dataLength;

	m_OLP_REMAIN_COUNT_SND.fetch_add(1);

	m_Write_OLP->ObjectId = m_ClientId;

	INT		ReturnValue	= WSASend(m_Socket,
		&WsaBuf,
		1,
		&WriteBytes,
		WriteFlag,
		&m_Write_OLP->Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_OLP_REMAIN_COUNT_SND.fetch_sub(1);
		Termination();

		return FALSE;
	}

	return TRUE;
}

}	}	}