#include "stdafx.h"

#include "GSBoard.h"
#include "BoardProcess.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"
#include "../DB/DBProxy/DBProcessContainer.h"
#include "../DB/DBJob/DBContext.h"

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
			{
				printf("delete room %d\n", pRoom->GetId());
				ROOMMGR.Del(pRoom);
			}
		}

		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);

		//매칭 큐에서 제거한다.
		ROOMMGR.GetMatchMap()[pPlayer->GetId()] = NULL;


		//로그아웃 쿼리를 날린다.
		boost::shared_ptr<RequestLogout> pRequest = ALLOCATOR.Create<RequestLogout>();
		pRequest->Account = pPlayer->m_Account.GetName();

		boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestLogout>>();
		PLAYER_MSG->pSession = pSession;
		PLAYER_MSG->pRequst = pRequest;
		PLAYER_MSG->Type = pSession->GetMyDBTP();
		PLAYER_MSG->SubType = ONQUERY;
		MAINPROC.RegisterCommand(PLAYER_MSG);

	}
	return TRUE;
}

VOID GSBoard::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<BoardProcess>();
}

BOOL GSBoard::DeleteAllConcurrentUser()
{
	//로그아웃 쿼리를 날린다.
	boost::shared_ptr<RequestDeleteAllConcurrentUser> pRequest = ALLOCATOR.Create<RequestDeleteAllConcurrentUser>();

	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestDeleteAllConcurrentUser>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestDeleteAllConcurrentUser>>();
	PLAYER_MSG->pRequst = pRequest;
	PLAYER_MSG->Type = MSG_TYPE_DB_1;
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

	return TRUE;
}



BOOL GSBoard::Initialize()
{
	INI.Load();

	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

	GSArgument _arg;

	_arg.m_AliveTime = 60; //sec
	_arg.m_BindPort  = m_Ini.Port;
	_arg.m_DBThreadCnt = 1;// dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
//	SetHandler<BoardProcess>();

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	DeleteAllConcurrentUser();
	
	return TRUE;
}
