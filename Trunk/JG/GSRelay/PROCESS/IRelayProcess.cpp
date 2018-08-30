#include "StdAfx.h"
#include "IRelayProcess.h"
#include "../GSPacket/Relay.h"
#include "GSSerialize.h"
#include "Sender.h"
#include "../group/GroupContainer.h"
#include "../iocp/UDP/ServerIOCP_UDP.h"
#include "../iocp/TCP/ServerIOCP_TCP.h"
#include "../GSIniContext.h"
#include "../Allocator/GSAllocator.h"


IRelayProcess::IRelayProcess(void)
{
	ADD_NET_FUNC(IRelayProcess,ID_GR_PKT_REGIST_RES,ACCESS_RELAYSVR);
	ADD_NET_FUNC(IRelayProcess,ID_CR_LOGIN_REQ,LOGIN_REQ);
	ADD_NET_FUNC(IRelayProcess,ID_CR_ENTER_ROOM_REQ,ENTER_ROOM_REQ);
	ADD_NET_FUNC(IRelayProcess,ID_CR_LEAVE_ROOM_REQ,LEAVE_ROOM_REQ);
}

IRelayProcess::~IRelayProcess(void)
{
}

VOID IRelayProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}

VOID IRelayProcess::ACCESS_RELAYSVR(LPVOID Data,DWORD Length)
{

	DESERIALIZE_BEGIN(Data,FR_PKT_REGIST_RES,pRes,Length)

	printf("Regist Success GSServer \n");

	DESERIALIZE_END
}

VOID IRelayProcess::CLOSE_RELAYSVR(LPVOID Data,DWORD Length)
{

}

VOID IRelayProcess::LOGIN_REQ(LPVOID Data,DWORD Length)
{
	DESERIALIZE_BEGIN(Data,CR_PKT_LOGIN_REQ,pRes,Length)

	DESERIALIZE_END
}


VOID IRelayProcess::ENTER_ROOM_REQ(LPVOID Data,DWORD Length)
{

}

VOID IRelayProcess::LEAVE_ROOM_REQ(LPVOID Data,DWORD Length)
{
}

VOID IRelayProcess::CREATE_UDP_SOCKET(LPVOID Data,DWORD Length)
{
	DESERIALIZE_BEGIN(Data,CR_PKT_LEAVE_ROOM_REQ,pRes,Length)

/*	RC_PKT_LEAVE_ROOM_RES snd;

	for(int i=0;i<MAX_UDP_SOCKET_COUNT;i++)
	{
		UDP_SESSION *pSession = IOCP_UDP.GetSessionMgr()->GetSession(i);
		if(pSession != NULL)
		{
			if( pSession->m_AssociateSocket.size() == 0)
			{
				snd.Port = INICONTEXT.m_UdpPort+i;

				TCP_SESSION *TCPSession = (TCP_SESSION *)pOwner;
				TCPSession->m_AssociateSocket.m_UdpRemoteInfo.sin_port = INICONTEXT.m_UdpPort+i;

				UDP_SESSION *UDPSession = IOCP_UDP.GetSessionMgr()->GetSession(INICONTEXT.m_UdpPort+i);
				TCPSession->m_AssociateSocket.m_UdpPair = (LPVOID *)UDPSession;

				UDPSession->m_TcpPair = (LPVOID *)TCPSession;
			}
		}
	}

	Sender::Send(pOwner,snd);*/

	DESERIALIZE_END
}


VOID IRelayProcess::BEGIN_STUN_REQ(LPVOID Data,DWORD Length)
{

}

VOID IRelayProcess::SUCCESS_STUN(LPVOID Data,DWORD Length)
{

}

VOID IRelayProcess::REPORT_STUN(LPVOID Data,DWORD Length)
{

}