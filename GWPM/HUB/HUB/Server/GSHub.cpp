#include "stdafx.h"

#include "GSHub.h"
#include "HubProcess.h"
#include "IUdpProcess.h"
#include "../PLAYER/PlayerContainer.h"
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
		
		ROOMMGR.DelMatchMap(pPlayer);
		ROOMMGR.LeaveRoomPlayer(pPlayer);
		
		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);
	

		//·Î±×¾Æ¿ô Äõ¸®¸¦ ³¯¸°´Ù.
	
		boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestLogout>>();
		PLAYER_MSG->pSession = pSession;
		PLAYER_MSG->pRequst.Index = pSession->GetPair();
		PLAYER_MSG->Type = pSession->GetMyDBTP();
		PLAYER_MSG->SubType = ONQUERY;
		MAINPROC.RegisterCommand(PLAYER_MSG);
	}



	return TRUE;
}

VOID GSBoard::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<HubProcess>();
}

BOOL GSBoard::DeleteAllConcurrentUser()
{
	//·Î±×¾Æ¿ô Äõ¸®¸¦ ³¯¸°´Ù.

	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestDeleteAllConcurrentUser>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestDeleteAllConcurrentUser>>();
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
	WORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

	GSArgument _arg;

	_arg.m_BindPort  = m_Ini.Port;
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;
	_arg.m_AliveTime = m_Ini.AliveSeconds * 1000;

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	Create(_arg);

	//default
	//
	SetHandler<IUdpProcess>();

	DeleteAllConcurrentUser();

	
	return TRUE;
}
