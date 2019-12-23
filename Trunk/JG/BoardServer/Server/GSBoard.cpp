#include "stdafx.h"

#include "GSBoard.h"
#include "BoardProcess.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"


GSBoard::GSBoard(void)
{
	Initialize();
}


GSBoard::~GSBoard(void)
{
}

BOOL GSBoard::Disconnect(GSCLIENT *pSession)
{
	PlayerPtr pPlayer = PLAYERMGR.Search(pSession->GetPair());
	if (pPlayer != NULL)
	{
		ROOM_PTR pRoom = ROOMMGR.Search(pPlayer->m_RoomNumber);
		if (pRoom)
		{
			pRoom->RemovePlayer(pPlayer);
			if (pRoom->GetCurrPlayer() == 0)
				ROOMMGR.Del(pRoom);
		}

		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);

	}
	return TRUE;
}

VOID GSBoard::Accept(GSCLIENT *object)
{

}


BOOL GSBoard::Initialize()
{
	INI.Load();

	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

	GSArgument _arg;

	_arg.m_AliveTime = 30; //sec
	_arg.m_BindPort  = m_Ini.Port;
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
	SetHandler<BoardProcess>();
	
	return TRUE;
}
