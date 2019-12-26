#pragma once

#include <boost/shared_ptr.hpp>
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
	virtual VOID		OnRead(boost::shared_ptr<GSClient> object, DWORD dataLength)	= 0;
	virtual VOID		OnWrote(boost::shared_ptr<GSClient> object, DWORD dataLength)	= 0;
	virtual VOID		OnConnected(boost::shared_ptr<GSClient> object)				= 0;
	virtual VOID		OnDisconnected(boost::shared_ptr<GSClient> object)			= 0;

public:
	BOOL				Initialize(VOID);
	BOOL				Termination(VOID);
	
	BOOL				RegIocpHandler(SOCKET socket, ULONG_PTR completionKey);

	VOID				WorkerThread(VOID);
	
	LONG				ExceptionFilter( LPEXCEPTION_POINTERS pExceptionInfo, LPCSTR szPosition );

};

}	}