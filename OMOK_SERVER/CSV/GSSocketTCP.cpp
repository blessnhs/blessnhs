#include "StdAfx.h"
#include "GSSocketTCP.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketTCP	{

GSSocketTCP::GSSocketTCP(void)
{
}

GSSocketTCP::~GSSocketTCP(void)
{
}


BOOL GSSocketTCP::Initialize(VOID)
{
	m_bConnected = FALSE;
	return TRUE;
}

BOOL GSSocketTCP::Termination(VOID)
{
	m_bConnected = FALSE;
	return GSSocket::Termination();
}

BOOL GSSocketTCP::GetPeerInfo(LPSTR szAddr, unsigned short &usPort)
{
	struct sockaddr_in addr;
	int addrsize = sizeof(addr);
	memset((void *)&addr, 0, addrsize);
	int nResult = getpeername(m_Socket, (sockaddr*)&addr, &addrsize);
	if(nResult == SOCKET_ERROR)
		return false;
	if(szAddr != NULL) {
		strcpy((char*)szAddr, inet_ntoa(addr.sin_addr));
		usPort = ntohs(addr.sin_port);
	}
	return TRUE;
}

BOOL GSSocketTCP::CreateTcpSocket(VOID)
{

	if (m_Socket)
		return FALSE;

	m_Socket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (m_Socket == INVALID_SOCKET)
		return FALSE;

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));
	return TRUE;
}

BOOL GSSocketTCP::InitializeReadForIocp(VOID)
{
	if (!m_Socket)
		return FALSE;

	WSABUF	WsaBuf;
	DWORD	ReadBytes	= 0;
	DWORD	ReadFlag	= 0;

	WsaBuf.buf			= (CHAR*) m_Buffer;
	WsaBuf.len			= MAX_BUFFER_LENGTH;

	INT		ReturnValue = WSARecv(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		&m_Read_OLP.Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		Termination();

		return FALSE;
	}

	return TRUE;
}

BOOL			GSSocketTCP::ReadForIocp(BYTE *data, DWORD &dataLength,DWORD RemainLength)
{

	if (!m_Socket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	if(RemainLength + dataLength >= MAX_BUFFER_LENGTH * 3)
		return FALSE;

	memcpy(data+RemainLength, m_Buffer, dataLength);

	return TRUE;
}

BOOL			GSSocketTCP::ReadForEventSelect(BYTE *data, DWORD &dataLength)
{

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

	INT		ReturnValue = WSARecv(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		&m_Read_OLP.Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		Termination();

		return FALSE;
	}

	memcpy(data, m_Buffer, ReadBytes);
	dataLength = ReadBytes;

	return TRUE;

}

VOID GSSocketTCP::SetConnected(BOOL bConnected) 
{
	m_bConnected = bConnected; 
}
BOOL GSSocketTCP::GetConnected(VOID) 
{ 
	return m_bConnected; 
}

BOOL GSSocketTCP::Write(BYTE *data, DWORD dataLength)
{
	
	if (!m_Socket)
		return FALSE;

	if (!data || dataLength <=0)
		return FALSE;

	WSABUF	WsaBuf;
	DWORD	WriteBytes	= 0;
	DWORD	WriteFlag	= 0;

	WsaBuf.buf			= (CHAR*) data;
	WsaBuf.len			= dataLength;

	INT		ReturnValue	= WSASend(m_Socket,
		&WsaBuf,
		1,
		&WriteBytes,
		WriteFlag,
		&m_Write_OLP.Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		Termination();

		return FALSE;
	}

	return TRUE;
}

}	}	}