#pragma once

#include "GSPacketUDP.h"
#include "GSIocp.h"
#include "UDPSession.h"

namespace GSRelayServer {	namespace UDPServerIOCP	{

typedef GSNetwork::GSSocket::GSPacket::GSPacketUDP::GSPacketUDP GSPacketUDP;

class CServerIOCP_UDP  : public GSNetwork::GSIocp::GSIocp
{
public:

	CServerIOCP_UDP(VOID);
	virtual ~CServerIOCP_UDP(VOID);

	BOOL Begin(VOID);
	VOID End(VOID);
	
protected:
	VOID OnRead(VOID *pObject, DWORD dwDataLength);
	VOID OnWrote(VOID *pObject, DWORD dwDataLength);
	VOID OnConnected(VOID *pObject);
	VOID OnDisconnected(VOID *pObject);

private:
	UDPNetworkSession::CNetworkSession *m_pListen[MAX_UDP_SOCKET_COUNT];

	HANDLE m_hKeepThread;
	HANDLE m_hKeepThreadDestroyEvent;

public:

	UDPNetworkSession::CNetworkSession		*GetListen(int pos);
	std::vector<WORD>		  m_FreePorts;
};

extern CServerIOCP_UDP &GetServerIOCP_UDP();

#define IOCP_UDP GSRelayServer::UDPServerIOCP::GetServerIOCP_UDP()

}	}