#pragma once

#include "GSPacketTCP.h"
#include "MessageMapDef.h"
#include "IClientSession.h"
#include "IProcess.h"
#include "IResponse.h"
#include "IMessage.h"
#include <boost/pool/object_pool.hpp>

namespace GSRelayServer	{	namespace	TCPSession	{	namespace	CNetworkSession	{

	struct TCPStock
	{
		DWORD Id;
		DWORD Key;
	};

	struct AssociateSocket
	{
		AssociateSocket()
		{
			memset(&m_UdpRemoteInfo,0,sizeof(m_UdpRemoteInfo));
			m_UdpPair = NULL;
			m_GroupNumber = m_AllocPort = 0;
		}

		SOCKADDR_IN					 m_UdpRemoteInfo;
		LPVOID						*m_UdpPair;

		DWORD						 m_GroupNumber;
		DWORD						 m_AllocPort;
		std::wstring						 m_Key;
	};

class CNetworkSession : public GSPACKETTCP, 
						public GSNetwork::IClientSession::IClientSession , public IResponse, public IHandler<IMessagePtr>
{
public:

	enum _eType
	{
		_PLAYER_,
		_FIELD_SERVER_,
		_GS_SERVER_,
	};

	CNetworkSession(void);
	~CNetworkSession(void);

	BOOL Recycle(SOCKET hListenSocket);

	void OnSend(WORD MainId,WORD SubId,char *Data,WORD Length);
	void OnRecv(DWORD Length);
	void OnDisconnect();
	virtual void OnConnect();

	IProcess<CNetworkSession> *GetProcess();
	void SetProcess(IProcess<CNetworkSession> *Process);
	
	virtual VOID OnResponse(LPVOID Data);

	void SetType(BYTE _Type);
	BYTE GetType();

	virtual void OnEvt(IMessagePtr Arg);

	virtual VOID MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength);

private:

	IProcess<CNetworkSession>								*m_Process;
	BYTE													m_Type;

public:

	TCPStock												m_Stock;
	AssociateSocket											m_AssociateSocket;

};

}	}	}

#define TCP_SESSION GSRelayServer::TCPSession::CNetworkSession::CNetworkSession