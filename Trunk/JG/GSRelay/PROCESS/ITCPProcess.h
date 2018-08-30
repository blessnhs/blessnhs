#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "../IOCP/TCP//TCPSession.h"

class ITCPProcess : public IProcess<TCP_SESSION>
{
public:
	ITCPProcess(void);
	~ITCPProcess(void);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

	VOID LOGIN_REQ(LPVOID Data,DWORD Length);

	VOID ENTER_ROOM_REQ(LPVOID Data,DWORD Length);
	VOID LEAVE_ROOM_REQ(LPVOID Data,DWORD Length);

	VOID SUCCESS_STUN(LPVOID Data,DWORD Length);
	VOID REPORT_STUN(LPVOID Data,DWORD Length);

	VOID BROADCAST_NTY(LPVOID Data,DWORD Length);

protected:

	DECLARE_MSG_MAP(ITCPProcess)
};

