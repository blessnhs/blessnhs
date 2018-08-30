#pragma once

#include "GSOdbc.h"
#include "IHandler.h"
#include "TCPSession.h"

namespace GSRelayServer	{	namespace	TCPNetWorkSessionMgr	{

class CNetWorkSessionMgr  : public GSGuard<CNetWorkSessionMgr> , public IHandler<IMessagePtr>, public IResponse
{
public:

	enum USER_EVNT_TYPE
	{
		ONACCEPT=0,
		ONPACKET,
		ONCLOSE,
		ONQUERY,
		ONTIME,
		ONQUERY_RES,
	};

	CNetWorkSessionMgr(void);
	~CNetWorkSessionMgr(void);

	BOOL Begin(SOCKET hListenSocket);
	VOID End(VOID);

	VOID WriteAll(DWORD dwProtocol, BYTE *pData, DWORD dwLength);

	virtual void OnEvt(IMessagePtr Arg);

	virtual VOID OnResponse(LPVOID Data);

	TCP_SESSION *GetSessionByAssociatePort(DWORD Port);

	TCP_SESSION *GetSession(DWORD Id);

private:

	std::vector<TCP_SESSION*> m_oConnectedSessionManager;

public:

	COdbc m_DB;
};

}	}

#define TCP_SESSION_MGR GSRelayServer::TCPNetWorkSessionMgr::CNetWorkSessionMgr