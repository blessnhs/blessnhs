#pragma once

#include "GSOdbc.h"

namespace GSDBA	{	namespace CNetWorkSessionMgr	{

typedef GSDBA::CNetworkSession::CNetworkSession	CNetworkSession;

class CNetWorkSessionMgr  : public GSGuard<CNetWorkSessionMgr>
{
public:
	CNetWorkSessionMgr(void);
	~CNetWorkSessionMgr(void);

	// �����ϴ� �Լ�
	BOOL Begin(SOCKET hListenSocket);
	// �����ϴ� �Լ�
	VOID End(VOID);

	// ���ӵ� ��� ConnectedSession�� ��Ŷ�� �����ϴ� �Լ�
	VOID WriteAll(DWORD dwProtocol, BYTE *pData, DWORD dwLength);

private:
	// ������ ConnectedSession��ü�� �����͸� �����ϴ� Vector
	std::vector<CNetworkSession*> m_vConnectedSessions;

public:

	COdbc m_DB;


};

}	}
