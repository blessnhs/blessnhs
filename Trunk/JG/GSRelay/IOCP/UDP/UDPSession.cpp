#include "StdAfx.h"
#include "UDPSession.h"

#include "ServerIOCP_UDP.h"
#include "../../../GSPacket/Protocol.h"
#include "../../GSMainProcess.h"
#include "../../MSG/MSG_PACKET_UDP.h"
#include "../../GSMainProcess.h"
#include "../../Allocator/GSAllocator.h"

namespace GSRelayServer	{	 namespace UDPNetworkSession	{

struct CNetworkSession::Private
{

};


CNetworkSession::CNetworkSession(void)
{
	m_Process = NULL;
	m_PairPlayerId  = 0;	
	m_AliveTime = 0;
	m_WillBeTerminated   = FALSE;
	m_DBStamp = 0;
	m_Channel = 0;

}

CNetworkSession::~CNetworkSession(void)
{
}

BOOL CNetworkSession::Recycle(SOCKET hListenSocket)
{
	CThreadSync Sync;

	Termination();

	m_AliveTime = 0;

	m_Process = NULL;
	m_PairPlayerId  = 0;	
	m_AliveTime = 0;
	m_WillBeTerminated   = FALSE;
	m_DBStamp = 0;
	m_bConnected = FALSE;

	return Initialize() && Accept(hListenSocket);

}

void CNetworkSession::SetChannel(WORD _channel)
{
	m_Channel = _channel;
}

WORD CNetworkSession::GetChannel()
{
	return m_Channel;
}


void CNetworkSession::OnSend(WORD MainId,WORD SubId,char *Data,WORD Length)
{
//	WritePacket(MainId,SubId,(BYTE*)Data,Length);
}

VOID CNetworkSession::MakeMsg(WORD MainProtocol, WORD SubProtocol,DWORD dataLength) 
{
	MSG_PACKET_UDP_PTR	pPlayerPacket(new MSG_PACKET_UDP);// = ALLOCATOR.Create<MSG_PACKET_UDP>(Type2Type<MSG_PACKET_UDP>());

	pPlayerPacket->pHandler = this;
	pPlayerPacket->Type	= GSRelayServer::GSMainProcess::GSMainProcess::MSG_TYPE_UDP_1;
	pPlayerPacket->SubType = ON_ACCEPT;
	pPlayerPacket->pSession = this;

	MAINPROC->RegisterCommand(pPlayerPacket);
}

void CNetworkSession::OnRecv(DWORD dwDataLength)
{
	WORD  MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;

	MakePacket(inet_ntoa(m_UdpRemoteInfo.sin_addr),m_UdpRemoteInfo.sin_port,dwDataLength,MainProtocol,SubProtocol,dwPacketLength);
}

void CNetworkSession::OnDisconnect()
{
}

void CNetworkSession::OnConnect()
{
}

IProcess<CNetworkSession>* CNetworkSession::GetProcess()
{
	return m_Process;
}

void CNetworkSession::SetProcess(IProcess<CNetworkSession> *Process)
{
	m_Process = Process;
	
	if(m_Process != NULL)
		m_Process->pOwner = this;
}

void CNetworkSession::SetType(BYTE _Type)
{
	m_Type = _Type;
}

BYTE CNetworkSession::GetType()
{
	return m_Type;
}

VOID CNetworkSession::OnResponse(LPVOID Data)
{
}

DWORD CNetworkSession::GetPair()
{
	return m_PairPlayerId;
}

void CNetworkSession::SetPair(DWORD Player)
{
	m_PairPlayerId = Player;
}

void CNetworkSession::SetAliveTime(DWORD Time)
{
	m_AliveTime = Time;
}

DWORD CNetworkSession::GetAliveTime()
{
	return m_AliveTime;
}

void CNetworkSession::IncDBStamp()
{
	++m_DBStamp;
}

void CNetworkSession::DecDBStamp()
{
	--m_DBStamp;
}

DWORD CNetworkSession::GetDBStamp()
{
	return m_DBStamp;
}

void CNetworkSession::SetWillBeTerminated(bool _terminate)
{
	m_WillBeTerminated = _terminate;
}

BOOL CNetworkSession::GetWillBeTerminated()
{
	return m_WillBeTerminated;
}

void CNetworkSession::OnEvt(IMessagePtr Arg)
{
	Arg->Execute(NULL);
}


}	}