#include "StdAfx.h"
#include "GSSocketUDP.h"
#include <winsock2.h>

namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketUDP	{

DWORD WINAPI ReliableUdpThreadCallback(LPVOID parameter)
{
	GSSocketUDP *Owner = (GSSocketUDP*) parameter;
	Owner->ReliableUdpThreadCallback();

	return 0;
}

GSSocketUDP::GSSocketUDP(void)
{
	memset(&m_UdpRemoteInfo, 0, sizeof(m_UdpRemoteInfo));
}

GSSocketUDP::~GSSocketUDP(void)
{
}

BOOL GSSocketUDP::Initialize(VOID)
{
//	memset(&m_UdpRemoteInfo, 0, sizeof(m_UdpRemoteInfo));
	GSSocket::Initialize();
	
	m_ReliableUdpThreadHandle		= NULL;
	m_ReliableUdpThreadStartupEvent	= NULL;
	m_ReliableUdpThreadDestroyEvent	= NULL;
	m_ReliableUdpThreadWakeUpEvent	= NULL;
	m_ReliableUdpWriteCompleteEvent	= NULL;

	m_IsReliableUdpSending			= FALSE;

	return TRUE;

}

BOOL GSSocketUDP::Termination(VOID)
{

	GSSocket::Termination();

	m_bConnected = FALSE;

	if (m_ReliableUdpThreadHandle)
	{
		SetEvent(m_ReliableUdpThreadDestroyEvent);

		WaitForSingleObject(m_ReliableUdpThreadHandle, INFINITE);

		CloseHandle(m_ReliableUdpThreadHandle);
	}

	if (m_ReliableUdpThreadDestroyEvent)
		CloseHandle(m_ReliableUdpThreadDestroyEvent);

	if (m_ReliableUdpThreadStartupEvent)
		CloseHandle(m_ReliableUdpThreadStartupEvent);

	if (m_ReliableUdpThreadWakeUpEvent)
		CloseHandle(m_ReliableUdpThreadWakeUpEvent);

	if (m_ReliableUdpWriteCompleteEvent)
		CloseHandle(m_ReliableUdpWriteCompleteEvent);

	while( !m_ReliableWriteQueue.empty() ) m_ReliableWriteQueue.pop();

	return TRUE;
}

BOOL GSSocketUDP::CreateUdpSocket(USHORT port)
{
	CThreadSync Sync;

	if (m_Socket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo;

	RemoteAddressInfo.sin_family			= AF_INET;
	RemoteAddressInfo.sin_port				= htons(port);
	RemoteAddressInfo.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	m_Socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_Socket == INVALID_SOCKET)
		return FALSE;

	if (::bind(m_Socket, (struct sockaddr*) &RemoteAddressInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		Termination();

		return FALSE;
	}

	// m_ReliableUdpThreadDestroyEvent
	m_ReliableUdpThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (m_ReliableUdpThreadDestroyEvent == NULL)
	{
		Termination();

		return FALSE;
	}
	//

	// m_ReliableUdpThreadStartupEvent
	m_ReliableUdpThreadStartupEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (m_ReliableUdpThreadStartupEvent == NULL)
	{
		Termination();

		return FALSE;
	}
	//

	// m_ReliableUdpThreadWakeUpEvent
	m_ReliableUdpThreadWakeUpEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (m_ReliableUdpThreadWakeUpEvent == NULL)
	{
		Termination();

		return FALSE;
	}
	//

	// m_ReliableUdpWriteCompleteEvent
	m_ReliableUdpWriteCompleteEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (m_ReliableUdpWriteCompleteEvent == NULL)
	{
		Termination();

		return FALSE;
	}
	//

	while( !m_ReliableWriteQueue.empty() ) m_ReliableWriteQueue.pop();

//	DWORD ReliableUdpThreadID	= 0;
//	m_ReliableUdpThreadHandle	= CreateThread(NULL, 0, GSNetwork::GSSocket::GSSocketUDP::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

// 	WaitForSingleObject(m_ReliableUdpThreadStartupEvent, INFINITE);

	return TRUE;
}

BOOL GSSocketUDP::InitializeReadFromForIocp(VOID)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		ReadBytes				= 0;
	DWORD		ReadFlag				= 0;
	INT			RemoteAddressInfoSize	= sizeof(m_UdpRemoteInfo);

	WsaBuf.buf			= (CHAR*)m_ReceiveBuffer;
	WsaBuf.len			= MAX_BUFFER_LENGTH;

	OVERLAPPED_EX* m_Read_OLP = new OVERLAPPED_EX;
	m_Read_OLP->IoType = IO_READ;
	m_Read_OLP->ObjectId = m_ClientId;
	m_OLP_REMAIN_COUNT_REC.fetch_add(1);

	INT		ReturnValue = WSARecvFrom(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		(SOCKADDR*) &m_UdpRemoteInfo,
		&RemoteAddressInfoSize,
		&m_Read_OLP->Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_OLP_REMAIN_COUNT_REC.fetch_sub(1);
		Termination();

		return FALSE;
	}

	return TRUE;
}

BOOL			GSSocketUDP::ReadFromForEventSelect(LPSTR remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!data)
		return FALSE;

	if (!m_Socket)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		ReadBytes				= 0;
	DWORD		ReadFlag				= 0;
	INT			RemoteAddressInfoSize	= sizeof(m_UdpRemoteInfo);

	WsaBuf.buf			= (CHAR*)m_ReceiveBuffer;
	WsaBuf.len			= MAX_BUFFER_LENGTH;
	
	
	OVERLAPPED_EX* m_Read_OLP = new OVERLAPPED_EX;
	m_Read_OLP->IoType = IO_READ;
	m_Read_OLP->ObjectId = m_ClientId;	


	m_OLP_REMAIN_COUNT_REC.fetch_add(1);

	INT		ReturnValue = WSARecvFrom(m_Socket,
		&WsaBuf,
		1,
		&ReadBytes,
		&ReadFlag,
		(SOCKADDR*) &m_UdpRemoteInfo,
		&RemoteAddressInfoSize,
		&m_Read_OLP->Overlapped,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_OLP_REMAIN_COUNT_REC.fetch_sub(1);
		Termination();

		return FALSE;
	}

	memcpy(data, m_ReceiveBuffer, ReadBytes);
	dataLength	= ReadBytes;

	//memcpy(remoteAddress, inet_ntoa(m_UdpRemoteInfo.sin_addr), 32);
	strcpy(remoteAddress, inet_ntoa(m_UdpRemoteInfo.sin_addr));
	remotePort	= ntohs(m_UdpRemoteInfo.sin_port);

	USHORT Ack = 0;
	memcpy(&Ack, m_ReceiveBuffer, sizeof(USHORT));

	if (Ack == 9999)
	{
		SetEvent(m_ReliableUdpWriteCompleteEvent);

		return FALSE;
	}
	else
	{
		Ack = 9999;
		WriteTo2(remoteAddress, remotePort, (BYTE*) &Ack, sizeof(USHORT));
	}

	return TRUE;
}

VOID GSSocketUDP::SetUdpWriteCompleteEvent(VOID)
{
	SetEvent(m_ReliableUdpWriteCompleteEvent);
}


BOOL	GSSocketUDP::ReadFromForIocp(LPSTR& remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	if(dataLength >= MAX_BUFFER_LENGTH)
		return FALSE;

	memcpy(data, m_ReceiveBuffer, dataLength);

	remoteAddress =  inet_ntoa(m_UdpRemoteInfo.sin_addr);
	remotePort	= ntohs(m_UdpRemoteInfo.sin_port);

/*	USHORT Ack = 0;
	memcpy(&Ack, m_Buffer, sizeof(USHORT));

	if (Ack == 9999)
	{
		SetEvent(m_ReliableUdpWriteCompleteEvent);

		return FALSE;
	}
	else
	{
		Ack = 9999;
		WriteTo2(remoteAddress, remotePort, (BYTE*) &Ack, sizeof(USHORT));
	}*/

	return TRUE;
}


BOOL GSSocketUDP::GetRemoteAddressAfterAccept(LPTSTR remoteAddress, USHORT &remotePort)
{
	CThreadSync Sync;

	if (!remoteAddress)
		return FALSE;

	sockaddr_in		*Local			= NULL;
	INT				LocalLength		= 0;

	sockaddr_in		*Remote			= NULL;
	INT				RemoteLength	= 0;

	GetAcceptExSockaddrs(m_ReceiveBuffer,
		0, 
		sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16,
		(sockaddr **) &Local,
		&LocalLength,
		(sockaddr **) &Remote,
		&RemoteLength);

	_tcscpy(remoteAddress, (LPTSTR)(inet_ntoa(Remote->sin_addr)));
	
	remotePort = ntohs(Remote->sin_port);

	return TRUE;
}

BOOL			GSSocketUDP::WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <=0)
		return FALSE;


	std::shared_ptr<WRITE_PACKET_INFO> pWriteData(new WRITE_PACKET_INFO);

	CopyMemory(pWriteData->Data,data,sizeof(pWriteData->Data));
	pWriteData->DataLength = dataLength;
	pWriteData->Object = this;
	
	m_ReliableWriteQueue.push(pWriteData); 

	if (!m_IsReliableUdpSending)
	{
		m_IsReliableUdpSending = TRUE;
		SetEvent(m_ReliableUdpThreadWakeUpEvent);
	}

	return TRUE;

}
BOOL			GSSocketUDP::WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!m_Socket)
		return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <=0)
		return FALSE;

	WSABUF		WsaBuf;
	DWORD		WriteBytes					= 0;
	DWORD		WriteFlag					= 0;

	SOCKADDR_IN	RemoteAddressInfo;
	INT			RemoteAddressInfoSize		= sizeof(RemoteAddressInfo);

	WsaBuf.buf								= (CHAR*) data;
	WsaBuf.len								= dataLength;

	RemoteAddressInfo.sin_family			= AF_INET;
	RemoteAddressInfo.sin_addr.S_un.S_addr	= inet_addr(remoteAddress);
    RemoteAddressInfo.sin_port				= htons(remotePort);

	OVERLAPPED_EX* m_Write_OLP = new OVERLAPPED_EX;
	m_Write_OLP->IoType = IO_WRITE;
	m_Write_OLP->ObjectId = m_ClientId;

	m_OLP_REMAIN_COUNT_SND.fetch_add(1);

	INT		ReturnValue	= WSASendTo(m_Socket,
		&WsaBuf,
		1,
		&WriteBytes,
		WriteFlag,
		(SOCKADDR*) &RemoteAddressInfo,
		RemoteAddressInfoSize,
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

VOID GSSocketUDP::ReliableUdpThreadCallback(VOID)
{
	DWORD				EventID					= 0;
	HANDLE				ThreadEvents[2]			= {m_ReliableUdpThreadDestroyEvent, m_ReliableUdpThreadWakeUpEvent};

	CHAR				RemoteAddress[32]		= {0,};
	USHORT				RemotePort				= 0;
	BYTE				Data[MAX_BUFFER_LENGTH]	= {0,};
	DWORD				DataLength				= 0;
	VOID				*Object					= NULL;

	while (TRUE)
	{
		SetEvent(m_ReliableUdpThreadStartupEvent);

        EventID = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
		switch (EventID)
		{
		case WAIT_OBJECT_0:
			return;

		case WAIT_OBJECT_0 + 1:
NEXT_DATA:

			std::shared_ptr<WRITE_PACKET_INFO> pWriteData = m_ReliableWriteQueue.front();
			// Write 일 경우 Pop을 해주고
//			if (m_ReliableWriteQueue.Pop(&Object, Data, DataLength, RemoteAddress, RemotePort))
			if(pWriteData != NULL)
			{
				// 데이터가 있을 경우
				// 실제 Write를 해 주고 WaitForSingleObject를 해 준다.
				// 받았을때 그 SetEvent를 해 주면 풀린다.
RETRY:
				if (!WriteTo2(pWriteData->RemoteAddress, pWriteData->RemotePort, pWriteData->Data, pWriteData->DataLength))
				{
					m_ReliableWriteQueue.pop();
					return;
				}
			
				m_ReliableWriteQueue.pop();

				DWORD Result = WaitForSingleObject(m_ReliableUdpWriteCompleteEvent, 10);

				// 받았을 경우 다음 데이터 처리
				if (Result == WAIT_OBJECT_0)
					goto NEXT_DATA;
				else
					goto RETRY;
			}
			else
				m_IsReliableUdpSending = FALSE;

			break;
		}
	}
}


}	}	}