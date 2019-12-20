#pragma once

#include "IPlayerProcess.h"

class BoardProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	BoardProcess(void);
	~BoardProcess(void);


	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

	VOID LOGIN_PLAYER(LPVOID Data, DWORD Length);

	VOID ROOM_CREATE(LPVOID Data, DWORD Length);
	VOID ROOM_ENTER(LPVOID Data, DWORD Length);
	VOID ROOM_LEAVE(LPVOID Data, DWORD Length);
	VOID ROOM_START(LPVOID Data, DWORD Length);
	VOID ROOM_READY(LPVOID Data, DWORD Length);
	VOID ROOM_CHAT(LPVOID Data, DWORD Length);
	VOID ALL_COMPLETE(LPVOID Data, DWORD Length);

	VOID AUTO_START(LPVOID Data, DWORD Length);

protected:

	DECLARE_MSG_MAP(BoardProcess)

};
