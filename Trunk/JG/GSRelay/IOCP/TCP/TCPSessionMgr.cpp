#include "StdAfx.h"
#include "TCPSessionMgr.h"
#include "./ServerIOCP_TCP.h"
#include "../GSPacket/Protocol.h"
#include "TCPSession.h"
#include "../../GSIniContext.h"


namespace GSRelayServer	{	namespace	TCPNetWorkSessionMgr	{


typedef GSRelayServer::TCPSession::CNetworkSession::CNetworkSession	CNetworkSession;

CNetWorkSessionMgr::CNetWorkSessionMgr(void)
{
}

CNetWorkSessionMgr::~CNetWorkSessionMgr(void)
{
}

TCP_SESSION *CNetWorkSessionMgr::GetSessionByAssociatePort(DWORD Port)
{
	DWORD MaxUser =INICONTEXT.m_MaxPlayerCount;

	for (DWORD i=0;i<MaxUser;i++)
	{
		if (m_oConnectedSessionManager[i]->m_AssociateSocket.m_UdpRemoteInfo.sin_port == Port)
		{
			return m_oConnectedSessionManager[i];
		}
	}

	return NULL;
}

TCP_SESSION *CNetWorkSessionMgr::GetSession(DWORD Id)
{
	if(Id <0 || Id >= INICONTEXT.m_MaxPlayerCount)
		return NULL;

	return m_oConnectedSessionManager[Id];
}


BOOL CNetWorkSessionMgr::Begin(SOCKET hListenSocket)
{
	if (!hListenSocket)
		return FALSE;
	
	DWORD MaxUser =INICONTEXT.m_MaxPlayerCount;

	for (DWORD i=0;i<MaxUser;i++)
	{
		TCP_SESSION *pConnectedSession = new TCP_SESSION();
		pConnectedSession->SetId(i);
		m_oConnectedSessionManager.push_back(pConnectedSession);
	}

	for (DWORD i=0;i<MaxUser;i++)
	{
		if (!m_oConnectedSessionManager[i]->Initialize())
		{
			CNetWorkSessionMgr::End();
			return FALSE;
		}

		if (!m_oConnectedSessionManager[i]->Accept(hListenSocket))
		{
			CNetWorkSessionMgr::End();
			return FALSE;
		}
	}

	return TRUE;
}

VOID CNetWorkSessionMgr::End(VOID)
{
	for (DWORD i=0;i<m_oConnectedSessionManager.size();i++)
	{
		m_oConnectedSessionManager[i]->Termination();

		delete m_oConnectedSessionManager[i];
	}
	m_oConnectedSessionManager.clear();
}

VOID CNetWorkSessionMgr::WriteAll(DWORD dwProtocol, BYTE *pData, DWORD dwLength)
{
	for (DWORD i=0;i<m_oConnectedSessionManager.size();i++)
	{
		if (m_oConnectedSessionManager[i]->GetConnected())
			m_oConnectedSessionManager[i]->WritePacket(dwProtocol, 0,pData, dwLength);
	}
}

VOID CNetWorkSessionMgr::OnResponse(LPVOID Data)
{
	GSObject *pData = (GSObject *)Data;

	switch(pData->GetId())
	{
	case _PROTOCOL_DBA::DBP_REQUEST_CHANNEL_LIST_RE:
		{
		}
		break;
	}

}

void CNetWorkSessionMgr::OnEvt(IMessagePtr Arg)
{
	Arg->Execute(NULL);
	return ;
	/*

	SvrMSG *msg = (SvrMSG *)Arg;

	switch(msg->SubType)
	{
	case USER_EVNT_TYPE::ONACCEPT:
		{
			MSG_ACCEPT *pProc = (MSG_ACCEPT *)msg;
			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_ACCEPTPool.destroy(pProc);
		}
		break;
	case USER_EVNT_TYPE::ONPACKET:
		{
			MSG_PACKET *pProc = (MSG_PACKET *)msg;
			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_PACKETPool.destroy(pProc);
		}
		break;
	case USER_EVNT_TYPE::ONCLOSE:
		{
			MSG_CLOSE *pProc = (MSG_CLOSE *)msg;
			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_CLOSEPool.destroy(pProc);
		}
		break;
	case USER_EVNT_TYPE::ONQUERY:
		{
			MSG_QUERY *pProc = (MSG_QUERY *)msg;
			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_QUERYPool.destroy(pProc);
		}
		break;
	case USER_EVNT_TYPE::ONTIME:
		{
			MSG_TIME *pProc = (MSG_TIME *)msg;

			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_TIMEPool.destroy(pProc);
		}
		break;
	case USER_EVNT_TYPE::ONQUERY_RES:
		{
			MSG_QUERY_RES *pProc = (MSG_QUERY_RES *)msg;
			pProc->Execute(NULL);

			GetAllocator().m_MSG_PLAYER_QUERY_RESPool.destroy(pProc);
		}
		break;
	default:
		break;
	}*/

}

}	}