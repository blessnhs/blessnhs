#include "StdAfx.h"
#include "TCPSession.h"
#include "../GSPacket/Relay.h"
#include "./ServerIOCP_TCP.h"
#include "../GSPacket/Protocol.h"
#include "GSPacketTCP.h"
#include "../../GSMainProcess.h"
#include "../../MSG/MSG_PACKET.h"
#include "../../MSG/MSG_CLOSE.h"
#include "../../GSMainProcess.h"
#include "../../Allocator/GSAllocator.h"
#include "../../GROUP/GroupContainer.h"
#include "../UDP/ServerIOCP_UDP.h"

namespace GSRelayServer	{	namespace	TCPSession	{	namespace	CNetworkSession	{

CNetworkSession::CNetworkSession(void)
{
	m_Process = NULL;
	m_Id = 0;
}

CNetworkSession::~CNetworkSession(void)
{
}

BOOL CNetworkSession::Recycle(SOCKET hListenSocket)
{
	CThreadSync Sync;

	Termination();

	return Initialize() && Accept(hListenSocket);

}

void CNetworkSession::OnSend(WORD MainId,WORD SubId,char *Data,WORD Length)
{
	WritePacket(MainId,SubId,(BYTE*)Data,Length);
}

VOID CNetworkSession::MakeMsg(WORD MainProtocol, WORD SubProtocol,DWORD dataLength) 
{
	MSG_PACKET_PTR	pPlayerPacket = ALLOCATOR.Create<MSG_PACKET>(Type2Type<MSG_PACKET>()); //(new MSG_PACKET);//

	pPlayerPacket->dwPacketLength = dataLength;
	pPlayerPacket->MainProtocol   = MainProtocol;
	pPlayerPacket->SubProtocol    = SubProtocol;
	pPlayerPacket->pHandler= this;
	pPlayerPacket->Type	= GSRelayServer::GSMainProcess::GSMainProcess::EVT_TYPE::MSG_TYPE_USER;
	pPlayerPacket->SubType = GSRelayServer::TCPNetWorkSessionMgr::CNetWorkSessionMgr::USER_EVNT_TYPE::ONPACKET;
	pPlayerPacket->pSession = this;

	MAINPROC->RegisterCommand(pPlayerPacket);
}

void CNetworkSession::OnRecv(DWORD dwDataLength)
{
	WORD  MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;

	MakePacket(dwDataLength,MainProtocol,SubProtocol,dwPacketLength);

//	MakeMsg(MainProtocol,SubProtocol,dwPacketLength);
}

void CNetworkSession::OnDisconnect()
{

	MSG_CLOSE_PTR	pPlayerClose = ALLOCATOR.Create<MSG_CLOSE>(Type2Type<MSG_CLOSE>()); //(new MSG_CLOSE);  //

	pPlayerClose->pSession	=	this;
	pPlayerClose->pListen	=	(TCP_SESSION *)IOCP_TCP.GetListen();
	pPlayerClose->pHandler= this;
	pPlayerClose->Type	= GSRelayServer::GSMainProcess::GSMainProcess::EVT_TYPE::MSG_TYPE_USER;
	pPlayerClose->SubType = GSRelayServer::TCPNetWorkSessionMgr::CNetWorkSessionMgr::USER_EVNT_TYPE::ONCLOSE;

	MAINPROC->RegisterCommand(pPlayerClose);

}

void CNetworkSession::OnConnect()
{
	printf("socket connected\n");
	
}

void CNetworkSession::SetType(BYTE _Type)
{
	m_Type = _Type;
}

BYTE CNetworkSession::GetType()
{
	return m_Type;
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

VOID CNetworkSession::OnResponse(LPVOID Data)
{
	GSObject *pData = (GSObject *)Data;

	switch(pData->GetId())
	{
	case _PROTOCOL_DBA::DBP_GET_CHAR_INFO_RE:
		{

		}
		break;
	}

}

void CNetworkSession::OnEvt(IMessagePtr Arg)
{
}


}	}	}