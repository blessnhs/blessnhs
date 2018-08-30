#pragma once

#include "GSPacketTCP.h"
#include "MessageMapDef.h"

namespace GSDBA	{	namespace CNetworkSession	{

class CNetworkSession : public GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP
{
public:
	CNetworkSession(void);
	~CNetworkSession(void);

	BOOL Recycle(SOCKET hListenSocket);

	VOID Process(LPVOID Data,DWORD Length,DWORD Protocol);

	VOID GET_LOGIN_INFO(LPVOID Data);

protected:
	DECLARE_MSG_MAP(CNetworkSession)
};

}	}