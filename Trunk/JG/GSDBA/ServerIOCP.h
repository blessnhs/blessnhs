#pragma once

namespace GSDBA	{	namespace	CServerIOCP	{

typedef GSDBA::CNetWorkSessionMgr::CNetWorkSessionMgr	CNetWorkSessionMgr;
typedef GSDBA::CNetworkSession::CNetworkSession	CNetworkSession;

class CServerIOCP  : public GSNetwork::GSIocp::GSIocp
{
public:
	
	CServerIOCP(VOID);
	virtual ~CServerIOCP(VOID);

	BOOL Begin(VOID);
	
	VOID End(VOID);

	VOID KeepThreadCallback(VOID);

protected:
	VOID OnRead(VOID *pObject, DWORD dwDataLength);
	VOID OnWrote(VOID *pObject, DWORD dwDataLength);
	VOID OnConnected(VOID *pObject);
	VOID OnDisconnected(VOID *pObject);

private:
	GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP *m_pListen;
	CNetWorkSessionMgr m_oConnectedSessionManager;

	HANDLE m_hKeepThread;
	HANDLE m_hKeepThreadDestroyEvent;
};

}	}