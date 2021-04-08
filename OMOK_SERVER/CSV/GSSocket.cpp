#include "StdAfx.h"
#include "GSSocket.h"

namespace GSNetwork	{ namespace GSSocket	{

GSSocket::GSSocket(VOID)
{
	memset(&m_Accept_OLP, 0, sizeof(m_Accept_OLP));
	memset(&m_Read_OLP, 0, sizeof(m_Read_OLP));
	memset(&m_Write_OLP, 0, sizeof(m_Write_OLP));

	memset(m_Buffer, 0, sizeof(m_Buffer));

	m_Socket							= NULL;

	m_Accept_OLP.IoType					= IO_ACCEPT;
	m_Read_OLP.IoType					= IO_READ;
	m_Write_OLP.IoType					= IO_WRITE;

	m_Accept_OLP.Object					= this;
	m_Read_OLP.Object					= this;
	m_Write_OLP.Object					= this;
	m_dwState							= STATUS_DISCONNECTED;
}

GSSocket::~GSSocket(VOID)
{

}

BOOL GSSocket::Initialize(VOID)
{

	if (m_Socket)
		return FALSE;

	memset(m_Buffer, 0, sizeof(m_Buffer));

	m_Socket							= NULL;
	return TRUE;
}

BOOL GSSocket::Termination(VOID)
{
	if (!m_Socket)
		return FALSE;

	closesocket(m_Socket);

	m_Socket					= NULL;
	m_dwState					= STATUS_DISCONNECTED;
	return TRUE;
}

BOOL GSSocket::Listen(USHORT port, INT backLog)
{
	
	if (port <= 0 || backLog <= 0)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	SOCKADDR_IN ListenSocketInfo;

	ListenSocketInfo.sin_family				= AF_INET;
	ListenSocketInfo.sin_port				= htons(port);
	ListenSocketInfo.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	if (bind(m_Socket, (struct sockaddr*) &ListenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
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
	if (!listenSocket)
		return FALSE;

	if (m_Socket)
		return FALSE;

	m_Socket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_Socket == INVALID_SOCKET)
	{
		Termination();

		return FALSE;
	}

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_Buffer, 
		0, 
		sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, 
		NULL, 
		&m_Accept_OLP.Overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL GSSocket::Accept2(SOCKET listenSocket)
{

	if (!listenSocket)
		return FALSE;

	if (m_Socket == INVALID_SOCKET)
	{
		Termination();

		return FALSE;
	}

	//BOOL NoDelay = TRUE;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_Buffer, 
		0, 
		sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, 
		NULL, 
		&m_Accept_OLP.Overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}


BOOL GSSocket::Connect(LPSTR address, USHORT port)
{
	if (!address || port <= 0)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo;

	RemoteAddressInfo.sin_family			= AF_INET;
	RemoteAddressInfo.sin_port				= htons(port);
	RemoteAddressInfo.sin_addr.S_un.S_addr	= inet_addr(address);

	if (WSAConnect(m_Socket, (LPSOCKADDR) &RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		int returnCode = WSAGetLastError();
		if (returnCode != WSAEWOULDBLOCK)
		{
			Termination();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL GSSocket::GetLocalIP(WCHAR* pIP)
{

	if(!m_Socket)
		return FALSE;

	//SOCKADDR_IN addr;
	//ZeroMemory(&addr, sizeof(addr));

	//int addrLength = sizeof(addr);
	//if(getsockname(m_Socket, (sockaddr*)&addr, &addrLength ) != SOCKET_ERROR)
	//{
	//	if(MultiByteToWideChar(CP_ACP, 0, inet_ntoa(addr.sin_addr), 32, pIP, 32) > 0)
	//		return TRUE;
	//}

	CHAR	Name[256]	= {0,};

	gethostname(Name, sizeof(Name));

	PHOSTENT host = gethostbyname(Name);
	if (host)
	{
		if(MultiByteToWideChar(CP_ACP, 0, inet_ntoa(*(struct in_addr*)*host->h_addr_list), -1, pIP, 32) > 0)
			return TRUE;
	}

	return FALSE;
}

USHORT GSSocket::GetLocalPort(VOID)
{

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