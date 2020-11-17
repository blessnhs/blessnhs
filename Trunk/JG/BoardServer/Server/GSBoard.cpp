#include "stdafx.h"

#include "GSBoard.h"
#include "BoardProcess.h"
#include "IUdpProcess.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"
#include "../DB/DBProxy/DBProcessContainer.h"
#include "../DB/DBJob/DBContext.h"
#include "../Channel/ChannelContainer.h"

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
		auto channel = CHANNELMGR.Search(pPlayer->GetChannel());
		if (channel != NULL)
		{
			channel->RoomContainer.DelMatchMap(pPlayer);
			channel->RoomContainer.LeaveRoomPlayer(pPlayer);
		}
		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);
	

		//채널 등록 유저 룸 제거
		CHANNELMGR.RoomLeaveChannel(pPlayer);
		CHANNELMGR.PlayerLeaveChannel(pPlayer);
		////////

		//로그아웃 쿼리를 날린다.
	
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
	object->SetHandler<BoardProcess>();
}

BOOL GSBoard::DeleteAllConcurrentUser()
{
	//로그아웃 쿼리를 날린다.

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
