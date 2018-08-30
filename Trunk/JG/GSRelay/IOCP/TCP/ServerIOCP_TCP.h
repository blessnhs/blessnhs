#pragma once

#include "GSPacketTCP.h"
#include "GSIocp.h"
#include "TCPSessionMgr.h"

namespace GSRelayServer {	namespace TCPServerIOCP	{

#define GSPacketTCP		GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP
#define TCPSessionMgr	GSRelayServer::TCPNetWorkSessionMgr::CNetWorkSessionMgr


class CServerIOCP_TCP  : public GSNetwork::GSIocp::GSIocp
{
public:

	CServerIOCP_TCP(VOID);
	virtual ~CServerIOCP_TCP(VOID);

	BOOL Begin(VOID);
	VOID End(VOID);
	
	TCP_SESSION_MGR* GetSessionMgr();

protected:
	VOID OnRead(VOID *pObject, DWORD dwDataLength);
	VOID OnWrote(VOID *pObject, DWORD dwDataLength);
	VOID OnConnected(VOID *pObject);
	VOID OnDisconnected(VOID *pObject);

private:
	GSPacketTCP *m_pListen;
	TCPSessionMgr *m_ConnectedSessionManager;

	HANDLE m_hKeepThread;
	HANDLE m_hKeepThreadDestroyEvent;

public:

	GSPacketTCP		*GetListen();

};

extern CServerIOCP_TCP &GetServerIOCP_TCP();

#define IOCP_TCP GSRelayServer::TCPServerIOCP::GetServerIOCP_TCP()

}	}