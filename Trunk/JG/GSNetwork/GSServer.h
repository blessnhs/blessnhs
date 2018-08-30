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

	virtual VOID												  OnRead(VOID *object, DWORD dataLength);
	virtual VOID												  OnWrote(VOID *object, DWORD dataLength);
	virtual VOID												  OnConnected(VOID *object);
	virtual VOID												  OnDisconnected(VOID *object);

	GSCLIENT*													  GetTcpListen();
	std::vector<GSCLIENT*>										 &GetUDPListenPorts();
	GSClientMgr::GSClientMgr									 &GetClientMgr();

	GSArgument													 &GetArgument();

	template<class U>
	VOID														  SetHandler()
	{
		for(int i=0;i<m_ClientMgr.m_Clients.size();i++)
		{
			boost::shared_ptr<U> Handler(new U);
			m_ClientMgr.m_Clients[i]->SetProcess(Handler);
		}
	}

	GSCLIENT*													  GetClient(DWORD _Id);

	virtual BOOL												  Disconnect(GSCLIENT *object) = 0;
	virtual VOID												  Accept(GSCLIENT *object) = 0;

private:

	GSClientMgr::GSClientMgr									  m_ClientMgr;

	GSCLIENT													 *m_pTCPListen;
	std::vector<GSCLIENT*>										  m_UDPListenPorts;
	std::vector<GSSocket::GSSocketUDP::GSSocketUDP*>			  m_FreePorts;

public:
	CRITICAL_SECTION											  m_PublicLock;

protected:
	GSArgument													  m_Arguments;
};

}	}

#define GSSERVER GSNetwork::GSServer::GSServer