#pragma once

class GSClient;

namespace GSNetwork	{	namespace GSIocp	{

class GSIocp
{
public:
	GSIocp(void);
	virtual ~GSIocp(void);

private:
	HANDLE				m_Handle;
	DWORD				m_WTCnt;

	std::vector<HANDLE>	m_WTVector;

	HANDLE				m_BeginEvtHandle;

protected:
	virtual VOID		OnRead(int client_id, DWORD dataLength)	= 0;
	virtual VOID		OnWrote(int client_id, DWORD dataLength)	= 0;
	virtual VOID		OnConnected(int client_id)				= 0;
	virtual VOID		OnDisconnected(int client_id, bool isForce = false)			= 0;
	virtual VOID		OnDisconnected2(int client_id,int type) = 0;

public:
	BOOL				Initialize(VOID);
	BOOL				Termination(VOID);
	
	BOOL				RegIocpHandler(SOCKET socket, ULONG_PTR completionKey);

	VOID				WorkerThread(VOID);
	
	LONG				ExceptionFilter( LPEXCEPTION_POINTERS pExceptionInfo, LPCSTR szPosition );

};

}	}