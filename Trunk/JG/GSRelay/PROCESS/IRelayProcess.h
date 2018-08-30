#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "../IOCP/TCP//TCPSession.h"

class IRelayProcess : public IProcess<TCP_SESSION>
{
public:
	IRelayProcess(void);
	~IRelayProcess(void);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);
	
	VOID ACCESS_RELAYSVR(LPVOID Data,DWORD Length);
	VOID CLOSE_RELAYSVR(LPVOID Data,DWORD Length);

	VOID LOGIN_REQ(LPVOID Data,DWORD Length);
	VOID ENTER_ROOM_REQ(LPVOID Data,DWORD Length);
	VOID LEAVE_ROOM_REQ(LPVOID Data,DWORD Length);

	VOID CREATE_UDP_SOCKET(LPVOID Data,DWORD Length);

	VOID BEGIN_STUN_REQ(LPVOID Data,DWORD Length);
	VOID SUCCESS_STUN(LPVOID Data,DWORD Length);
	VOID REPORT_STUN(LPVOID Data,DWORD Length);


protected:

	DECLARE_MSG_MAP(IRelayProcess)
};

