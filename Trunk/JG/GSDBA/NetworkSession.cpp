#include "StdAfx.h"
#include "NetworkSession.h"
#include "../GSPacket/protocol.h"

namespace GSDBA	{	namespace CNetworkSession	{

CNetworkSession::CNetworkSession(void)
{
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

VOID CNetworkSession::Process(LPVOID Data,DWORD Length,DWORD Protocol)
{
//	NET_FUNC_EXE(Protocol,Data);
}


VOID CNetworkSession::GET_LOGIN_INFO(LPVOID Data)
{
/*
	MW_PKT_USER_LOGIN RecvData;

	GSBuffer buff;
	buff.SetBuffer((BYTE*)Data);
	buff.ReadUSHORT(&RecvData.wSid);
	buff.ReadUSHORT(&RecvData.wChannelID);
	buff.ReadBytes((BYTE*)RecvData.szUserID,MAX_ID_LENGTH);
	buff.ReadBytes((BYTE*)RecvData.szSessionKey,MAX_SESSION_KEY_SIZE);

	WORD Result = GetDBProcess().ProcedureUserLogin(&RecvData);


	MW_PKT_USER_LOGIN_RE SendData;
	SendData.wSid    =   RecvData.wSid;
	SendData.wResult =   Result;

	WritePacket(DBP_GET_LOGIN_INFO_RE,0,(BYTE*)&SendData,sizeof(SendData));
*/
}

}	}