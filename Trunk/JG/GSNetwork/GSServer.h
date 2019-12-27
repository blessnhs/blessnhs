#pragma once

#include "GSArgument.h"
#include "GSIocp.h"
#include "GSClientMgr.h"
#include <vector>
#include "SingleTonHolder.h"

namespace GSNetwork	{	namespace GSServer	{


typedef GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;

class GSServer : public GSIocp::GSIocp , public Singleton<GSServer>
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

	GSServer(void);
	~GSServer(void);

	bool														  Create(GSArgument arg);

	BOOL														  BeginTCP();
	BOOL														  BeginUDP();
	VOID														  End(VOID);

	virtual VOID												  OnRead(int client_id, DWORD dataLength);
	virtual VOID												  OnWrote(int client_id, DWORD dataLength);
	virtual VOID												  OnConnected(int client_id);
	virtual VOID												  OnDisconnected(int client_id);

	GSCLIENT_PTR												  GetTcpListen();
	std::vector<GSCLIENT*>										 &GetUDPListenPorts();
	GSClientMgr::GSClientMgr									 &GetClientMgr();

	GSArgument													 &GetArgument();

	template<class U>
	VOID														  SetHandler()
	{
		for each (auto client in m_ClientMgr.m_Clients)
		{
			if (client.second == NULL)
				continue;

				boost::shared_ptr<U> Handler(new U);
				client.second->SetProcess(Handler);
		}
	}

	GSCLIENT_PTR												  GetClient(DWORD _Id);
	int															  IncClinetId();

	virtual BOOL												  Disconnect(GSCLIENT_PTR object) = 0;
	virtual VOID												  Accept(GSCLIENT_PTR object) = 0;

private:

	GSClientMgr::GSClientMgr									  m_ClientMgr;

	GSCLIENT_PTR												  m_pTCPListen;
	std::vector<GSCLIENT*>										  m_UDPListenPorts;
	std::vector<GSSocket::GSSocketUDP::GSSocketUDP*>			  m_FreePorts;

public:
	CRITICAL_SECTION											  m_PublicLock;

protected:
	GSArgument													  m_Arguments;
};

}	}

#define GSSERVER GSNetwork::GSServer::GSServer