#include "StdAfx.h"
#include "GSSocket.h"
#include <WinSock2.h>
#include <mstcpip.h>

namespace GSNetwork	{ namespace GSSocket	{

GSSocket::GSSocket(VOID)
{
	m_OLP_REMAIN_COUNT_ACC = 0;
	m_OLP_REMAIN_COUNT_REC = 0;
	m_OLP_REMAIN_COUNT_SND = 0;

	memset(m_ReceiveBuffer, 0, sizeof(m_ReceiveBuffer));
	m_Socket							= NULL;

	m_bConnected = FALSE;

	m_ClientId = -1;
}

BYTE *GSSocket::GetBuffer()
{
	return m_ReceiveBuffer;
}

GSSocket::~GSSocket(VOID)
{
}

BOOL GSSocket::Initialize(VOID)
{
	CThreadSync Sync;

	m_bConnected = FALSE;

	if (m_Socket)
		return FALSE;

	memset(m_ReceiveBuffer, 0, sizeof(m_ReceiveBuffer));

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

	int option = 1;

	setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));

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

SOCKET GSSocket::Connect2(LPSTR address, USHORT port)
{
	CThreadSync Sync;

	if (!m_Socket)
		m_Socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);;

	SOCKADDR_IN RemoteAddressInfo;
	ZeroMemory(&RemoteAddressInfo, sizeof(RemoteAddressInfo));

	RemoteAddressInfo.sin_family = AF_INET;
	RemoteAddressInfo.sin_port = htons(port);
	RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);




	if (m_Socket == INVALID_SOCKET)
	{
		Termination();

		return FALSE;
	}

	OVERLAPPED_EX* m_Accept_OLP = new OVERLAPPED_EX;
	m_Accept_OLP->IoType = IO_ACCEPT;	
	m_Accept_OLP->ObjectId = m_ClientId;

	struct linger lingerOpt;
	lingerOpt.l_onoff = 1;
	lingerOpt.l_linger = 0;

	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&lingerOpt, sizeof(lingerOpt));

	int opt = 1;
	//		if(setsockopt(m_hSocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&opt, sizeof(opt))==SOCKET_ERROR)
	//			return false;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&opt, sizeof(opt)) == SOCKET_ERROR)
		return false;
	struct sockaddr_in addr;
	memset((void*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address);
	if (addr.sin_addr.s_addr == INADDR_NONE) {
		LPHOSTENT lpHosten = gethostbyname(address);
		if (lpHosten == NULL) return false;
		addr.sin_addr.s_addr = ((LPIN_ADDR)lpHosten->h_addr)->s_addr;
	}
	if (WSAConnect(m_Socket, (LPSOCKADDR)&RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	//if (connect(m_Socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		return false;

	return m_Socket;
}

BOOL GSSocket::SetSocketOption(int socket_id, int keep_alive, int keep_idle, int keep_count, int keep_intvl)
{
	DWORD dwRet;
	tcp_keepalive tcpkl;
	tcpkl.onoff = 1;
	tcpkl.keepalivetime = 5000; //5초마다 신호를 보냄
	tcpkl.keepaliveinterval = 1000;	// keep alive 신호를 받고 응답이 없으면 1초마다 재전송

	WSAIoctl(socket_id, SIO_KEEPALIVE_VALS, &tcpkl, sizeof(tcp_keepalive), 0, 0, &dwRet, NULL, NULL);



	//onoff = 1, l_longer = 0 closesocket시 강제 종료 남은 데이터 버림 TIME_WAIT남지 않음
	//onoff = 1, l_long != 0 blocking방식 종료처리 (graceful shutdown)
	//onoff = 0 //정상적 종료처리 종료처리 시점을 알수가 없다

	//LINGER ling;
	//ling.l_onoff = 1;
	//ling.l_linger = 0;

	//int ret = setsockopt(socket_id, SOL_SOCKET, SO_LINGER, (const char FAR*) & ling, sizeof(ling));
	//if (ret == -1)
	//	printf("fail keep alive socket opt\n");

	//서버입장에서는 2분의 time_wait지속 시간을 갖는 리소스 낭비이며 장애 발생 원인이 된다. 따라서 time_wait이 없는 abortive shutdonw으로 longer를 사용해야한다.

	/*int ret = setsockopt(socket_id, SOL_SOCKET, SO_KEEPALIVE, (const char FAR*) & keep_alive, sizeof(keep_alive));
	if (ret == -1)
		printf("fail keep alive socket opt\n");

	ret = setsockopt(socket_id, IPPROTO_TCP, TCP_KEEPIDLE, (const char FAR*) & keep_idle, sizeof(keep_idle));
	if (ret == -1)
		printf("fail keep_idle socket opt\n");

	ret = setsockopt(socket_id, IPPROTO_TCP, TCP_KEEPCNT, (const char FAR*) & keep_count, sizeof(keep_count));
	if (ret == -1)
		printf("fail keep_count socket opt\n");

	ret = setsockopt(socket_id, IPPROTO_TCP, TCP_KEEPINTVL, (const char FAR*) & keep_intvl, sizeof(keep_intvl));
	if (ret == -1)
		printf("fail keep_intvl socket opt\n");*/

	return TRUE;
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

	OVERLAPPED_EX* m_Accept_OLP = new OVERLAPPED_EX;
	m_Accept_OLP->IoType = IO_ACCEPT;
	m_Accept_OLP->ObjectId = m_ClientId;

	SetSocketOption(m_Socket);

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_ReceiveBuffer,
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

	OVERLAPPED_EX* m_Accept_OLP = new OVERLAPPED_EX;
	m_Accept_OLP->IoType = IO_ACCEPT;
	m_Accept_OLP->ObjectId = m_ClientId;

	SetSocketOption(m_Socket);

	if (!AcceptEx(listenSocket, 
		m_Socket, 
		m_ReceiveBuffer,
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
	RemoteAddressInfo.sin_addr.S_un.S_addr	= inet_addr(address);

	//inet_pton(AF_INET, address, &RemoteAddressInfo.sin_addr.S_un.S_addr);

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

BOOL GSSocket::GetLocalIP(std::string& ipaddress, int& port)
{
	CThreadSync Sync;

	if(!m_Socket)
		return FALSE;


	sockaddr_in* Local = NULL;
	INT				LocalLength = 0;

	sockaddr_in* Remote = NULL;
	INT				RemoteLength = 0;

	GetAcceptExSockaddrs(m_ReceiveBuffer,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr**)&Local,
		&LocalLength,
		(sockaddr**)&Remote,
		&RemoteLength);

//	char remoteAddress[64];

//	_tcscpy(remoteAddress, (LPTSTR)(inet_ntoa(Remote->sin_addr)));

	ipaddress = (LPTSTR)(inet_ntoa(Remote->sin_addr));

	port = ntohs(Remote->sin_port);


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