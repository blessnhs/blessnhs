#include "StdAfx.h"

#include "BoardProcess.h"
#include "../Player/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"

using namespace google;

BoardProcess::BoardProcess(void)
{
	ADD_NET_FUNC(BoardProcess, ID_PKT_VERSION_REQ, VERSION);
	ADD_NET_FUNC(BoardProcess, ID_PKT_LOGIN_REQ, LOGIN_PLAYER);
	ADD_NET_FUNC(BoardProcess, ID_PKT_CREATE_ROOM_REQ, ROOM_CREATE);
	ADD_NET_FUNC(BoardProcess, ID_PKT_ENTER_ROOM_REQ, ROOM_ENTER);
	ADD_NET_FUNC(BoardProcess, ID_PKT_LEAVE_ROOM_REQ, ROOM_LEAVE);
	ADD_NET_FUNC(BoardProcess, ID_PKT_BROADCAST_ROOM_MESSAGE_REQ, ROOM_CHAT);
	ADD_NET_FUNC(BoardProcess, ID_PKT_ROOM_LIST_REQ, ROOM_LIST);
	ADD_NET_FUNC(BoardProcess, ID_PKT_MATCH_REQ, MATCH);
}


BoardProcess::~BoardProcess(void)
{
}


VOID BoardProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client)
{
	try
	{	//로그인 하지 않은 유저가 패킷을 요청 했을때
		// 버전이나 로그인 패킷이 아닌 경우 처리하지 않는다.
		if (MainProtocol != ID_PKT_VERSION_REQ && MainProtocol != ID_PKT_LOGIN_REQ)
		{
			PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
			if (pPlayer == NULL)
			{
#ifdef _DEBUG
#else
				return;
#endif


			}
		}

		NET_FUNC_EXE(MainProtocol, Data, Length, Client);
	}
	catch (int exception)
	{
		printf("handle exception %d\n",exception);
	}
}

VOID BoardProcess::VERSION(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(VERSION_REQ, version)

	//로그인 쿼리를 날린다.
	boost::shared_ptr<RequestVersion> pRequest = ALLOCATOR.Create<RequestVersion>();
	
	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestVersion>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestVersion>>();
	PLAYER_MSG->pSession = Client;
	PLAYER_MSG->pRequst = pRequest;
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID BoardProcess::LOGIN_PLAYER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(LOGIN_REQ,login)
		
	printf("result %d id %s pwd %s\n", _result_,login.var_uid().c_str(), login.var_token().c_str());
	

	//로그인 쿼리를 날린다.
	boost::shared_ptr<RequestPlayerAuth> pRequest = ALLOCATOR.Create<RequestPlayerAuth>();
	pRequest->Uid.assign(login.var_uid().begin(), login.var_uid().end());
	pRequest->Token.assign(login.var_token().begin(), login.var_token().end());

	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestPlayerAuth>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestPlayerAuth>>();
	PLAYER_MSG->pSession = Client;
	PLAYER_MSG->pRequst = pRequest;
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY; 
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID BoardProcess::ROOM_CREATE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	DECLARE_RECV_TYPE(CREATE_ROOM_REQ, createroom)
	
	CREATE_ROOM_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	if (pPlayer == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, pOwner)
		return;
	}

	if (createroom.var_name().size() == 0)
	{
		return;
	}

	ROOM_PTR RoomPtr = ROOMMGR.Create();
	RoomPtr->m_Stock.Name = createroom.var_name();

	ROOMMGR.Add(RoomPtr);
	RoomPtr->m_Stock.MAX_PLAYER = USHRT_MAX;

	if (pPlayer != NULL)
	{
		pPlayer->SetRoom(RoomPtr->GetId());
	}

	RoomPtr->InsertPlayer(pPlayer);

	res.set_var_room_id(RoomPtr->GetId());
	res.mutable_var_name()->assign(RoomPtr->m_Stock.Name);
	SEND_PROTO_BUFFER(res, pOwner)

	RoomPtr->SendNewUserInfo(pPlayer);	//방에 있는 유저들에게 새로운 유저 정보전송
}

VOID BoardProcess::ROOM_ENTER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	DECLARE_RECV_TYPE(ENTER_ROOM_REQ, enterroom)

	ENTER_ROOM_RES res;

	ROOM_PTR RoomPtr = ROOMMGR.Search(enterroom.var_id());
	if (RoomPtr == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, pOwner)
		return;
	}	
	
	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	//이미 입장 해 있다면 
	if (RoomPtr->FindPlayer(pPlayer) == TRUE)
	{
		return;
	}
	
	RoomPtr->InsertPlayer(pPlayer);

	pPlayer->SetRoom( RoomPtr->GetId() );

	res.set_var_room_id(RoomPtr->GetId());
	res.set_var_name(RoomPtr->m_Stock.Name.c_str());
	SEND_PROTO_BUFFER(res, pOwner)

	//새로 입장한 유저에게 방안의 유저 정보전송
	for each (auto iter in RoomPtr->m_PlayerMap)
	{
		if (iter.second == NULL)
			continue;

		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo* userinfo = nty.mutable_var_room_user();

		userinfo->mutable_var_name()->assign(iter.second->m_Char[0].GetName());
		SEND_PROTO_BUFFER(nty, pOwner)
	}
	

	//방안의 유저들 에게 새로운 유저 정보를 전송
	for each (auto iter in RoomPtr->m_PlayerMap)
	{
		if (iter.second == NULL)
			continue;
	
		GSCLIENT_PTR pPair = SERVER.GetClient(iter.second->GetPair());
		if (pPair == NULL)
			continue;

		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo* userinfo = nty.mutable_var_room_user();


		userinfo->mutable_var_name()->assign(pPlayer->m_Char[0].GetName());
		SEND_PROTO_BUFFER(nty, pPair)
	}
}

VOID BoardProcess::ROOM_LEAVE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	DECLARE_RECV_TYPE(LEAVE_ROOM_REQ, leaveroom)

	LEAVE_ROOM_RES res;
	
	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	ROOM_PTR RoomPtr = ROOMMGR.Search(pPlayer->GetRoom());
	if (RoomPtr != NULL)
	{
		if (RoomPtr->FindPlayer(pPlayer) != USHRT_MAX)
		{
			res.set_var_code(Success);
			res.mutable_var_name()->assign(pPlayer->m_Account.GetName());

			RoomPtr->SendToAll(res);

			RoomPtr->RemovePlayer(pPlayer);
			pPlayer->SetRoom(0);

			if (RoomPtr->GetCurrPlayer() == 0)
				ROOMMGR.Del(RoomPtr);
		}

		pPlayer->m_Char[0].SetAllComplete(FALSE);
		pPlayer->m_Char[0].SetReady(FALSE);
	}
}

VOID BoardProcess::ROOM_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//if (pPlayer != NULL)
	//{
	//	ROOM_PTR pPtr = ROOMMGR.Search(pPlayer->m_RoomNumber);
	//	if (pPtr != NULL)
	//	{
	//		DECLARE_JSON_WRITER
	//		Json::FastWriter writer;
	//		std::string outputConfig = writer.write(root2);

	//		pPtr->SendToAll(ID_CF_PKT_START_ROOM_REQ, (BYTE *)outputConfig.c_str(), outputConfig.size());
	//	}
	//}
}

VOID BoardProcess::ROOM_READY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//if (pPlayer != NULL)
	//{
	//	ROOM_PTR pPtr = ROOMMGR.Search(pPlayer->m_RoomNumber);
	//	if (pPtr != NULL)
	//	{
	//		if (pPlayer->m_Char[0].GetReady() == FALSE)
	//			pPlayer->m_Char[0].SetReady(TRUE);
	//		else
	//			pPlayer->m_Char[0].SetReady(FALSE);

	//		FC_PKT_READY_ROOM_RES snd;
	//		snd.Name = pPlayer->m_Char[0].GetName();
	//		snd.Result = pPlayer->m_Char[0].GetReady();

	//		DECLARE_JSON_WRITER
	//		ADD_JSON_WSTR_MEMBER("Name", pPlayer->m_Char[0].GetName())
	//			ADD_JSON_MEMBER("Result", pPlayer->m_Char[0].GetReady())

	//			Json::FastWriter writer;
	//		std::string outputConfig = writer.write(root2);

	//		pPtr->SendToAll(ID_FC_PKT_READY_ROOM_RES, (BYTE *)outputConfig.c_str(), outputConfig.size());
	//	}
	//}

}

VOID BoardProcess::ROOM_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	DECLARE_RECV_TYPE(BROADCAST_ROOM_MESSAGE_REQ, message)

	BROADCAST_ROOM_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	if (message.var_message().length() > 1024 || message.var_message().length() <= 0)
	{
		return;
	}

	{
		res.mutable_var_message()->assign(message.var_message());
		res.mutable_var_name()->assign(pPlayer->m_Account.GetName());

		ROOM_PTR pPtr = ROOMMGR.Search(pPlayer->GetRoom());
		if (pPtr != NULL)
		{
			pPtr->SendToAll(res);
		}
	}

}

VOID BoardProcess::ROOM_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ROOM_LIST_REQ, roomlistreq)

	ROOM_LIST_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	for each (auto& room in ROOMMGR.GetRoomMap())
	{
		if (room.second == NULL)
			continue;

		RoomInfo2* info = res.mutable_var_room_list()->Add();
	
		info->set_var_id(room.first);
		info->set_var_name(room.second->m_Stock.Name);
		info->set_var_current_count(room.second->GetCurrPlayer());
		info->set_var_max_count(-1);
	}

	SEND_PROTO_BUFFER(res, Client)
}

VOID BoardProcess::MATCH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(MATCH_REQ, roomlistreq)

	MATCH_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	if (pPlayer->GetRoom() != 0)
	{
		return;
	}

	res.set_var_code(Success);
	SEND_PROTO_BUFFER(res, Client)



	for each(auto player in ROOMMGR.GetMatchMap())
	{
		if (player.second == NULL)
			continue;

		if (player.first == pPlayer->GetId())
			continue;

		ROOMMGR.CreateMatchRoom(player.second, pPlayer);

		return;
	}


	//여기 있으면 큐에 아무도 없는것임
	ROOMMGR.GetMatchMap()[pPlayer->GetId()] = pPlayer;
}


VOID BoardProcess::AUTO_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//DECLARE_JSON_READ
	//GET_JSON_UINT_MEMBER("GameType", GameType)

	//ROOM_PTR RoomPtr = ROOMMGR.SearchByGameType(GameType);
	//if (RoomPtr == NULL)
	//{
	//	DECLARE_JSON_WRITER

	//	std::string Name = _T("AutoRoom");
	//	ADD_JSON_WSTR_MEMBER("Name", Name)

	//		ADD_JSON_MEMBER("MaxCount", 2)
	//		ADD_JSON_MEMBER("Type", GameType)
	//		Json::FastWriter writer;
	//	std::string outputConfig = writer.write(root2);

	//	ROOM_CREATE((char*)outputConfig.c_str(), outputConfig.size(), pOwner);
	//}
	//else
	//{
	//	DECLARE_JSON_WRITER
	//	ADD_JSON_MEMBER("RoomNumber", (WORD)RoomPtr->GetId())
	//		Json::FastWriter writer;
	//	std::string outputConfig = writer.write(root2);
	//	ROOM_ENTER((char*)outputConfig.c_str(), outputConfig.size(), pOwner);
	//}

}

VOID BoardProcess::ALL_COMPLETE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//pPlayer->m_Char[0].SetAllComplete(TRUE);

	//ROOM_PTR pPtr = ROOMMGR.Search(pPlayer->m_RoomNumber);
	//if (pPtr != NULL)
	//{
	//	if (pPtr->IsAllComplete() == true)
	//	{
	//		DECLARE_JSON_WRITER

	//		Json::FastWriter writer;
	//		std::string outputConfig = writer.write(root2);

	//		pPtr->SendToAll(ID_FC_PKT_ALL_COMPLETE, (BYTE *)outputConfig.c_str(), outputConfig.size());
	//	}
	//}
}
