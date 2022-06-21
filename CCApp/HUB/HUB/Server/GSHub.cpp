#include "stdafx.h"

#include "GSHub.h"
#include "HubProcess.h"
#include "IUdpProcess.h"
#include "../PLAYER/PlayerContainer.h"
#include "../Room/RoomContainer.h"
#include "../DB/DBProxy/DBProcessContainer.h"

class RequestDeleteAllConcurrentUser;

GSHub::GSHub(void)
{
}


GSHub::~GSHub(void)
{
}

BOOL GSHub::Disconnect(GSCLIENT_PTR pSession)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pSession->GetPair());
	//if (pPlayer != NULL)
	//{
	//	
	//	for each (auto room in pPlayer->m_Char[0].GetRoom())
	//	{
	//		ROOMMGR.LeaveRoomPlayer(pPlayer, room);
	//	}

	//	
	//	pPlayer->SetPair(ULONG_MAX);
	//	PLAYERMGR.Del(pPlayer);
	//}

	////·Î±×¾Æ¿ô Äõ¸®¸¦ ³¯¸°´Ù.
	//boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestLogout>>();
	//PLAYER_MSG->pSession = pSession;

	//PLAYER_MSG->Request.m_args = std::tuple<INT64>(pSession->GetPair());

	//PLAYER_MSG->Type = pSession->GetMyDBTP();
	//PLAYER_MSG->SubType = ONQUERY;
	//MAINPROC.RegisterCommand(PLAYER_MSG);

	printf("disconnect front session \n");



	return TRUE;
}

VOID GSHub::Accept(GSCLIENT_PTR object)
{
	object->SetHandler<HubProcess>();
}

BOOL GSHub::DeleteAllConcurrentUser()
{
	//·Î±×¾Æ¿ô Äõ¸®¸¦ ³¯¸°´Ù.

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestDeleteAllConcurrentUser>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestDeleteAllConcurrentUser>>();
	PLAYER_MSG->Type = MSG_TYPE_DB_1;
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

	return TRUE;
}



BOOL GSHub::Initialize()
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
	_arg.m_UseCompress = true;

	DBPROCESSCONTAINER_CER.Initialize(_arg.m_DBThreadCnt);

	Create(_arg);

	//default
	//
	SetHandler<HubProcess>();

	DeleteAllConcurrentUser();

	ROOMMGR.CreateDBRoomList();


	
	return TRUE;
}
