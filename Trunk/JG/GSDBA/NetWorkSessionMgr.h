#pragma once

#include "GSOdbc.h"

namespace GSDBA	{	namespace CNetWorkSessionMgr	{

typedef GSDBA::CNetworkSession::CNetworkSession	CNetworkSession;

class CNetWorkSessionMgr  : public GSGuard<CNetWorkSessionMgr>
{
public:
	CNetWorkSessionMgr(void);
	~CNetWorkSessionMgr(void);

	// 시작하는 함수
	BOOL Begin(SOCKET hListenSocket);
	// 종료하는 함수
	VOID End(VOID);

	// 접속된 모든 ConnectedSession에 패킷을 전송하는 함수
	VOID WriteAll(DWORD dwProtocol, BYTE *pData, DWORD dwLength);

private:
	// 생성된 ConnectedSession개체의 포인터를 관리하는 Vector
	std::vector<CNetworkSession*> m_vConnectedSessions;

public:

	COdbc m_DB;


};

}	}
