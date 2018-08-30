#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "../IOCP/UDP/UDPSession.h"

class IUdpProcess : public IProcess<UDP_SESSION>
{
public:
	IUdpProcess(void);
	~IUdpProcess(void);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);
	
	VOID ACCESS_RELAYSVR(LPVOID Data,DWORD Length);
	VOID CLOSE_RELAYSVR(LPVOID Data,DWORD Length);

	VOID BROADCAST_NTY(LPVOID Data,DWORD Length);

	VOID CREATE_UDP_SOCKET_NTY(LPVOID Data,DWORD Length);


protected:

	DECLARE_MSG_MAP(IUdpProcess)
};

