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
		ROOMMGR.LeaveRoomPlayer(pPlayer);

		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);

		//��Ī ť���� �����Ѵ�.
		ROOMMGR.DelMatchMap(pPlayer);
	}

	//�α׾ƿ� ������ ������.
	boost::shared_ptr<RequestLogout> pRequest = ALLOCATOR.Create<RequestLogout>();
	pRequest->Index = pSession->GetPair();

	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestLogout>>();
	PLAYER_MSG->pSession = pSession;
	PLAYER_MSG->pRequst = pRequest;
	PLAYER_MSG->Type = pSession->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

	return TRUE;
}

VOID GSBoard::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<BoardProcess>();
}

BOOL GSBoard::DeleteAllConcurrentUser()
{
	//�α׾ƿ� ������ ������.
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
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = dwThreadNum;
	_arg.m_MaxClient = m_Ini.MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
//	SetHandler<BoardProcess>();

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	DeleteAllConcurrentUser();
	
	return TRUE;
}
