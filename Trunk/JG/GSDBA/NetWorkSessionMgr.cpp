#include "StdAfx.h"
#include "NetWorkSessionMgr.h"

namespace GSDBA	{	namespace CNetWorkSessionMgr	{

CNetWorkSessionMgr::CNetWorkSessionMgr(void)
{
}

CNetWorkSessionMgr::~CNetWorkSessionMgr(void)
{
}

BOOL CNetWorkSessionMgr::Begin(SOCKET hListenSocket)
{
	if (!hListenSocket)
		return FALSE;

	for (DWORD i=0;i<MAX_USER;i++)
	{
		CNetworkSession *pConnectedSession = new CNetworkSession();
		m_vConnectedSessions.push_back(pConnectedSession);
	}

	for (DWORD i=0;i<MAX_USER;i++)
	{
		if (!m_vConnectedSessions[i]->Initialize())
		{
			CNetWorkSessionMgr::End();
			return FALSE;
		}

		if (!m_vConnectedSessions[i]->Accept(hListenSocket))
		{
			CNetWorkSessionMgr::End();
			return FALSE;
		}
	}

	return TRUE;
}

VOID CNetWorkSessionMgr::End(VOID)
{
	for (DWORD i=0;i<m_vConnectedSessions.size();i++)
	{
		m_vConnectedSessions[i]->Termination();

		delete m_vConnectedSessions[i];
	}
	m_vConnectedSessions.clear();
}

VOID CNetWorkSessionMgr::WriteAll(DWORD dwProtocol, BYTE *pData, DWORD dwLength)
{
	for (DWORD i=0;i<m_vConnectedSessions.size();i++)
	{
		if (m_vConnectedSessions[i]->GetConnected())
			m_vConnectedSessions[i]->WritePacket(dwProtocol,0, pData, dwLength);
	}
}

}	}