#pragma once

#include "GSArgument.h"
#include "GSIocp.h"
#include "GSClientMgr.h"


namespace GSNetwork	{	namespace GSServer	{


typedef GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;

class GSServer : public GSIocp::GSIocp , public Singleton<GSServer>, public GSGuard<GSServer>
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
	virtual VOID												  OnDisconnected(int client_id,bool isForce = false);

	virtual VOID 												  OnDisconnected2(int client_id, int type);

	GSCLIENT_PTR												  GetTcpListen();
	concurrency::concurrent_unordered_map<DWORD, GSCLIENT_PTR>	 &GetUDPListenPorts();
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

	virtual BOOL												  Disconnect(GSCLIENT_PTR object) = 0;
	virtual VOID												  Accept(GSCLIENT_PTR object) = 0;

public:
	atomic<unsigned long long>		TotalRecvBytes;

	atomic<unsigned long long>		TotalSendBytes;


private:


	GSClientMgr::GSClientMgr									  m_ClientMgr;

	GSCLIENT_PTR												  m_pTCPListen;
	concurrency::concurrent_unordered_map<DWORD,GSCLIENT_PTR>	  m_UDPListenPorts;
	concurrency::concurrent_queue<boost::shared_ptr<GSSocket::GSSocketUDP::GSSocketUDP>>			  m_FreePorts;

protected:
	GSArgument													  m_Arguments;
	int															  m_EvtClientId;
};

}	}

#define GSSERVER GSNetwork::GSServer::GSServer