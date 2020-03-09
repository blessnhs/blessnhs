#include "stdafx.h"

#include "GSBoard.h"
#include "BoardProcess.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"
#include "../DB/DBProxy/DBProcessContainer.h"

GSBoard::GSBoard(void)
{
}


GSBoard::~GSBoard(void)
{
}

BOOL GSBoard::Disconnect(GSCLIENT_PTR pSession)
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

VOID GSBoard::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<BoardProcess>();
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
	_arg.m_DBThreadCnt = 2;
	_arg.m_LogicThreadCnt = 512;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
//	SetHandler<BoardProcess>();

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);
	
	return TRUE;
}
