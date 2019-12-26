#pragma once

#include "GSGuard.h"
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
	OVERLAPPED_EX	m_Accept_OLP;
	OVERLAPPED_EX	m_Read_OLP;
	OVERLAPPED_EX	m_Write_OLP;

protected:
	
	BYTE			m_Buffer[MAX_BUFFER_LENGTH];

	SOCKET			m_Socket;

	BOOL			m_bConnected;

public:
	BOOL			Initialize(VOID);
	BOOL			Termination(VOID);

	BOOL			Listen(USHORT port, INT backLog);
	BOOL			Accept(SOCKET listenSocket);
	BOOL			Accept2(SOCKET listenSocket);

	BOOL			GetLocalIP(WCHAR* pIP);
	USHORT			GetLocalPort(VOID);
	
	BOOL			Connect(LPSTR address, USHORT port);

	SOCKET			GetSocket(VOID);
	
	long			ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, char *Info);
	long			ExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo, DWORD Info);

	BYTE *GetBuffer();
};

}	}