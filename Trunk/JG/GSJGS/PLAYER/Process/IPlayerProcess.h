#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "../NetworkSession.h"

namespace GSFrontServer {	namespace IPlayerProcess	{

class IPlayerProcess : public IProcess<SESSION>
{
public:
	IPlayerProcess(void);
	~IPlayerProcess(void);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);
	
	VOID ACCESS_GAMESVR(LPVOID Data,DWORD Length);

	VOID REGIST_PLAYER(LPVOID Data,DWORD Length);
	VOID CHOOSE_CHARACTER(LPVOID Data,DWORD Length);
	VOID CLOSE_PLAYER(LPVOID Data,DWORD Length);
	VOID CREATE_CHARACTER(LPVOID Data,DWORD Length);

	VOID ENTER_CHANNEL(LPVOID Data,DWORD Length);

	VOID LOBBY_CHAT(LPVOID Data,DWORD Length);

	VOID ROOM_LIST(LPVOID Data,DWORD Length);
	VOID USER_LIST(LPVOID Data,DWORD Length);

	VOID ROOM_CREATE(LPVOID Data,DWORD Length);
	VOID ROOM_ENTER(LPVOID Data,DWORD Length);
	VOID ROOM_LEAVE(LPVOID Data,DWORD Length);
	VOID ROOM_START(LPVOID Data,DWORD Length);
	VOID ROOM_READY(LPVOID Data,DWORD Length);
	VOID ROOM_CHAT(LPVOID Data,DWORD Length);
	VOID ALL_COMPLETE(LPVOID Data,DWORD Length);

	VOID AUTO_START(LPVOID Data,DWORD Length);


	VOID Move(LPVOID Data,DWORD Length);
	
protected:

	DECLARE_MSG_MAP(IPlayerProcess)
};

}	}