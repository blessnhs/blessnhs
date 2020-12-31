#include "StdAfx.h"
#include "GSSocket.h"
#include <WinSock2.h>

namespace GSNetwork	{ namespace GSSocket	{

GSSocket::GSSocket(VOID)
{
	m_Accept_OLP = new OVERLAPPED_EX();
	m_Read_OLP = new OVERLAPPED_EX();
	m_Write_OLP = new OVERLAPPED_EX();

	m_OLP_REMAIN_COUNT_ACC = 0;
	m_OLP_REMAIN_COUNT_REC = 0;
	m_OLP_REMAIN_COUNT_SND = 0;

	memset(m_Buffer, 0, sizeof(m_Buffer));
	m_Socket							= NULL;

	m_Accept_OLP->IoType				= IO_ACCEPT;
	m_Read_OLP->IoType					= IO_READ;
	m_Write_OLP->IoType					= IO_WRITE;

	m_bConnected = FALSE;

	m_ClientId = -1;
}

BYTE *GSSocket::GetBuffer()
{
	return m_Buffer;
}

GSSocket::~GSSocket(VOID)
{
	delete m_Accept_OLP;
	delete m_Read_OLP;
	delete m_Write_OLP;

	m_Accept_OLP = NULL;
	m_Read_OLP = NULL;
	m_Write_OLP = NULL;
}

BOOL GSSocket::Initialize(VOID)
{
	CThreadSync Sync;

	m_bConnected = FALSE;

	if (m_Socket)
		return FALSE;

	memset(m_Buffer, 0, sizeof(m_Buffer));

	m_Socket							= NULL;
	return TRUE;
}

BOOL GSSocket::Termination(VOID)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	closesocket(m_Socket);

	m_Socket					= NULL;
	return TRUE;
}

BOOL GSSocket::Listen(USHORT port, INT backLog)
{
	CThreadSync Sync;

	if (port <= 0 || backLog <= 0)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	SOCKADDR_IN ListenSocketInfo;

	ListenSocketInfo.sin_family				= AF_INET;
	ListenSocketInfo.sin_port				= htons(port);
	ListenSocketInfo.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	if (::bind(m_Socket, (struct sockaddr*) &ListenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		Termination();

		return FALSE;
	}

	if (listen(m_Socket, backLog) == SOCKET_ERROR)
	{
		Termination();

		return FALSE;
	}

	LINGER Linger;
	Linger.l_onoff	= 1;
	Linger.l_linger = 0;

	if (setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*) &Linger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		Termination();

		return FALSE;
	}

	return TRUE;
}

SOCKET GSSocket::GetSocket(VOID)
{
	return m_Socket; 
}

BOOL GSSocket::Accept(SOCKET listenSocket)
{
	CThreadSync Sync;

	if (!listenSocket)
		return FALSE;

	if (m_Socket)
		return FALSE;

	m_Socket	= WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_Socket == INVALID_SOCKET)
	{
		Termination();

		return FALSE;
	}

	m_OLP_REMAIN_COUNT_ACC.fetch_add(1);

	m_Accept_OLP->ObjectId = m_ClientId;

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_Buffer, 
		0, 
		sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, 
		NULL, 
		&m_Accept_OLP->Overlapped))
	{
		int ErrorCode = WSAGetLastError();
		if (ErrorCode != ERROR_IO_PENDING && ErrorCode != WSAEWOULDBLOCK)
		{
			m_OLP_REMAIN_COUNT_ACC.fetch_sub(1);
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL GSSocket::Accept2(SOCKET listenSocket)
{
	CThreadSync Sync;

	if (!listenSocket)
		return FALSE;

	if (m_Socket == INVALID_SOCKET)
	{
		Termination();

		return FALSE;
	}

	m_OLP_REMAIN_COUNT_ACC.fetch_add(1);

	m_Accept_OLP->ObjectId = m_ClientId;

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_Buffer, 
		0, 
		sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, 
		NULL, 
		&m_Accept_OLP->Overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			m_OLP_REMAIN_COUNT_ACC.fetch_sub(1);
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}


BOOL GSSocket::Connect(LPSTR address, USHORT port)
{
	CThreadSync Sync;

	if (!address || port <= 0)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo;

	RemoteAddressInfo.sin_family			= AF_INET;
	RemoteAddressInfo.sin_port				= htons(port);
//	RemoteAddressInfo.sin_addr.S_un.S_addr	= inet_addr(address);

	inet_pton(AF_INET, address, &RemoteAddressInfo.sin_addr.S_un.S_addr);

	if (WSAConnect(m_Socket, (LPSOCKADDR) &RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		int returnCode  = WSAGetLastError();
		if (returnCode != WSAEWOULDBLOCK)
		{
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL GSSocket::GetLocalIP(std::string& ipaddress)
{
	CThreadSync Sync;

	if(!m_Socket)
		return FALSE;

	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));

	CHAR	Name[256]	= {0,};
	int client_len = sizeof(addr);

	getsockname(m_Socket, (struct sockaddr*)&addr,&client_len);

	//char ip[128];
	//inet_ntop(AF_INET, &(addr.sin_addr), (PSTR)ip, 128);

	//InetNtop(AF_INET, &(addr), (PSTR)ip, 128);
	//ipaddress = ip;

	inet_ntoa(addr.sin_addr);

	return FALSE;
}

USHORT GSSocket::GetLocalPort(VOID)
{
	CThreadSync Sync;

	if (!m_Socket)
		return 0;

	SOCKADDR_IN Addr;
	ZeroMemory(&Addr, sizeof(Addr));

	INT AddrLength = sizeof(Addr);
	if (getsockname(m_Socket, (sockaddr*) &Addr, &AddrLength) != SOCKET_ERROR)
		return ntohs(Addr.sin_port);

	return 0;
}

long GSSocket::ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, char *Info)
{
	PEXCEPTION_RECORD pRecord = pExceptionInfo->ExceptionRecord;


	char strExceptionDescription[512];
	sprintf_s( strExceptionDescription, "The system takes place the exception in the 0x%p while run function of %s,error code: 0x%08X",
									   (pRecord->ExceptionAddress), 
									   Info,
		                               pRecord->ExceptionCode);

	return EXCEPTION_EXECUTE_HANDLER;
}

long GSSocket::ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, DWORD Info)
{
	PEXCEPTION_RECORD pRecord = pExceptionInfo->ExceptionRecord;


	char strExceptionDescription[512];
	sprintf_s( strExceptionDescription, "The system takes place the exception in the 0x%p while run function of 0x%08X,error code: 0x%08X",
									   (pRecord->ExceptionAddress), 
		                               Info,
		                               pRecord->ExceptionCode);

	return EXCEPTION_EXECUTE_HANDLER;
}


}	}