#pragma once

#include "GSPacketUDP.h"
#include "IClientSession.h"
#include "IProcess.h"
#include "IResponse.h"
#include "IHandler.h"
#include "IMessage.h"
#include <boost/pool/object_pool.hpp>

namespace GSRelayServer	{	 namespace UDPNetworkSession	{

	typedef GSNetwork::GSSocket::GSPacket::GSPacketUDP::GSPacketUDP GSPacketUDP;

	struct AssociateSocket
	{
		AssociateSocket()
		{
			memset(&m_UdpRemoteInfo,0,sizeof(m_UdpRemoteInfo));
		}

		SOCKADDR_IN						 m_UdpRemoteInfo;
	
	};

	struct UDPSTUNSTOCK
	{
		bool IsReady;
	};

class CNetworkSession : public GSPacketUDP  , public IResponse, public IHandler<IMessagePtr>
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
	void OnConnect();

	IProcess<CNetworkSession> *GetProcess();
	void SetProcess(IProcess<CNetworkSession> *Process);

	void SetType(BYTE _Type);
	BYTE GetType();

	virtual VOID OnResponse(LPVOID Data);
	DWORD GetPair();

	void SetPair(DWORD Player);

	virtual void OnEvt(IMessagePtr Arg);

	void SetAliveTime(DWORD Time);
	DWORD GetAliveTime();

	void IncDBStamp();
	void DecDBStamp();
	DWORD GetDBStamp();

	void SetWillBeTerminated(bool _terminate);
	BOOL GetWillBeTerminated();
		
	virtual VOID MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength);

	void SetChannel(WORD _channel);
	WORD GetChannel();

public:

	std::vector<AssociateSocket>							 m_AssociateSocket;

	LPVOID													*m_TcpPair;

	UDPSTUNSTOCK											 m_STUNStock;

private:

	IProcess<CNetworkSession>								*m_Process;

	BYTE													 m_Type;

	DWORD 													 m_PairPlayerId;
	DWORD													 m_AliveTime;

	BOOL													 m_WillBeTerminated;
	DWORD													 m_DBStamp;
	WORD													 m_Channel;
public:

	struct					Private;
	Private *				This;

	int id;
};

}	}

#define UDP_SESSION GSRelayServer::UDPNetworkSession::CNetworkSession