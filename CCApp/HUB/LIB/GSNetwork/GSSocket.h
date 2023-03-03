#pragma once

#include "GSSocketBase.h"

namespace GSNetwork	{ namespace GSSocket	{

// Socket Status
#define STATUS_DISCONNECTED		0x00
#define STATUS_CONNECTED		0x01
#define STATUS_LISTENED			0x02

class GSSocket : public GSGuard<GSSocket>
{
public:
	GSSocket(VOID);
	virtual ~GSSocket(VOID);

public:

	std::atomic<int>		m_OLP_REMAIN_COUNT_ACC;
	std::atomic<int>		m_OLP_REMAIN_COUNT_REC;
	std::atomic<int>		m_OLP_REMAIN_COUNT_SND;
		
	int				m_ClientId;

protected:
	
					//wsarecvieve에서 이번에 전송받은 패킷 버퍼
	BYTE			m_ReceiveBuffer[MAX_BUFFER_LENGTH];

	SOCKET			m_Socket;

	BOOL			m_bConnected;

public:
	BOOL			Initialize(VOID);
	BOOL			Termination(VOID);

	BOOL			Listen(USHORT port, INT backLog);
	BOOL			Accept(SOCKET listenSocket);
	BOOL			Accept2(SOCKET listenSocket);



	BOOL			GetLocalIP(std::string& ipaddress,int &port);
	USHORT			GetLocalPort(VOID);
	
	BOOL			Connect(LPSTR address, USHORT port);
	SOCKET			Connect2(LPSTR address, USHORT port);

	SOCKET			GetSocket(VOID);
	
	long			ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, char *Info);
	long			ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, DWORD Info);

	BYTE *GetBuffer();
};

}	}