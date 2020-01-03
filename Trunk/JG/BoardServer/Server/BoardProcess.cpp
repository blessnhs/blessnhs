#include "StdAfx.h"

#include "BoardProcess.h"
#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "../DB/DBProxy/DBProcess.h"
#include "../DB/DBProxy/DBProcessContainer.h"
#include "../DB/DBJob/DBContext.h"

#include "GSAllocator.h"
#include "GSMainProcess.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"

#include "MessageMapDef.h"

#include "GSBoard.h"

#include "../MainProcess/MSG/MSG_PLAYER_QUERY.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <boost/make_shared.hpp>

#include "CLI.GS.pb.h"

using namespace google;

BoardProcess::BoardProcess(void)
{
	ADD_NET_FUNC(BoardProcess, ID_PKT_LOGIN_REQ, LOGIN_PLAYER);
	ADD_NET_FUNC(BoardProcess, ID_PKT_ALL_COMPLETE_NTY, ALL_COMPLETE);
	ADD_NET_FUNC(BoardProcess, ID_PKT_AUTO_START_REQ, AUTO_START);
	ADD_NET_FUNC(BoardProcess, ID_PKT_CREATE_ROOM_REQ, ROOM_CREATE);
	ADD_NET_FUNC(BoardProcess, ID_PKT_ENTER_ROOM_REQ, ROOM_ENTER);
	ADD_NET_FUNC(BoardProcess, ID_PKT_LEAVE_ROOM_REQ, ROOM_LEAVE);
	ADD_NET_FUNC(BoardProcess, ID_PKT_CHAT_ROOM_REQ, ROOM_CHAT);
}


BoardProcess::~BoardProcess(void)
{
}

VOID BoardProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client)
{
	try
	{
		NET_FUNC_EXE(MainProtocol, Data, Length, Client);
	}
	catch (int exception)
	{
		printf("handle exception\n");
	}
}

VOID BoardProcess::LOGIN_PLAYER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	LOGIN_REQ login;
	bool result = login.ParseFromArray((char*)Data, Length);
	if (result == false)
		return;
	
	printf("result %d id %s pwd %s\n", result,login.var_id().c_str(), login.var_passwd().c_str());
	

	//로그인 쿼리를 날린다.
	boost::shared_ptr<RequestPlayerAuth> pRequest = ALLOCATOR.Create<RequestPlayerAuth>();
	pRequest->Account.assign(login.var_id().begin(), login.var_id().end());
	pRequest->Passwd.assign(login.var_passwd().begin(), login.var_passwd().end());

	boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestPlayerAuth>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestPlayerAuth>>();
	PLAYER_MSG->pSession = pOwner;
	PLAYER_MSG->pRequst = pRequest;
	PLAYER_MSG->Type = pOwner->GetMyDBTP(MSG_TYPE_DB_1);
	PLAYER_MSG->SubType = ONQUERY; 
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID BoardProcess::ROOM_CREATE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//if (pPlayer == NULL || pPlayer->m_RoomNumber != 0)
	//{
	//	DECLARE_JSON_WRITER
	//	ADD_JSON_MEMBER("Result", FC_PKT_CREATE_ROOM_RES::ErrorCode::NOT_FOUND_PLAYER)
	//	END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_CREATE_ROOM_RES)
	//	return;
	//}

	//DECLARE_JSON_READ
	//GET_JSON_WSTR_MEMBER("Name", Name)
	//GET_JSON_INT_MEMBER("Type", Type)
	//GET_JSON_INT_MEMBER("MaxCount", MaxCount)

	//ROOM_PTR RoomPtr = ROOMMGR.Create();
	//RoomPtr->m_Stock.Name = Name;

	//ROOMMGR.Add(RoomPtr);
	//RoomPtr->m_Stock.MAX_PLAYER = MaxCount;

	//if (pPlayer != NULL)
	//{
	//	pPlayer->m_RoomNumber = RoomPtr->GetId();
	//}

	//RoomPtr->InsertPlayer(pPlayer);

	//DECLARE_JSON_WRITER
	//ADD_JSON_MEMBER("Result", 0)
	//ADD_JSON_MEMBER("RoomNumber", (WORD)RoomPtr->GetId())
	//ADD_JSON_MEMBER("MaxCount", RoomPtr->m_Stock.MAX_PLAYER)
	//ADD_JSON_WSTR_MEMBER("Name", RoomPtr->m_Stock.Name)
	//END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_CREATE_ROOM_RES)

	//RoomPtr->SendNewUserInfo(pPlayer);	//방에 있는 유저들에게 새로운 유저 정보전송
}

VOID BoardProcess::ROOM_ENTER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	//DECLARE_JSON_READ
	//GET_JSON_UINT_MEMBER("RoomNumber", RoomNumber)

	//ROOM_PTR RoomPtr = ROOMMGR.Search(RoomNumber);
	//if (RoomPtr != NULL)
	//{
	//	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	//	if (pPlayer != NULL)
	//	{
	//		RoomPtr->InsertPlayer(pPlayer);

	//		pPlayer->m_RoomNumber = RoomPtr->GetId();
	//	}

	//	DECLARE_JSON_WRITER
	//	ADD_JSON_MEMBER("Result", 0)

	//		FC_PKT_ENTER_ROOM_RES res;
	//	res.Result = FC_PKT_ENTER_ROOM_RES::SUCCESS;
	//	int pos = 0;
	//	res.roominfo.Index = RoomPtr->GetId();
	//	res.roominfo.Name = RoomPtr->m_Stock.Name;

	//	root2["roominfo"]["Index"] = (WORD)RoomPtr->GetId();
	//	root2["roominfo"]["Name"] = RoomPtr->m_Stock.Name.c_str();

	//	Json::Value Array;
	//	res.Result = 0;
	//	root2["PlayerList"] = Array;

	//	END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_ENTER_ROOM_RES)

	//		std::map<DWORD, PLAYER_PTR>::iterator iter = RoomPtr->m_PlayerMap.begin();
	//	while (iter != RoomPtr->m_PlayerMap.end())
	//	{
	//		Array["Pos"] = (WORD)iter->first;
	//		root2["PlayerName"] = iter->second->m_Account.GetName().c_str();

	//		DECLARE_JSON_WRITER
	//		ADD_JSON_MEMBER("Pos", pos)

	//			wstring Name = iter->second->m_Account.GetName();
	//		ADD_JSON_WSTR_MEMBER("PlayerName", Name)
	//			END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_NEW_USER_IN_ROOM)

	//			res.Pos[pos] = iter->first;
	//		res.PlayerName[pos] = iter->second->m_Account.GetName();

	//		iter++;
	//		pos++;
	//	}

	//	iter = RoomPtr->m_PlayerMap.begin();
	//	while (iter != RoomPtr->m_PlayerMap.end())
	//	{
	//		//방안의 유저들 정보를 새로운 유저에게 전송

	//		if (iter->second->m_Account.GetName() != pPlayer->m_Account.GetName())
	//		{
	//			GSCLIENT_PTR pPair = SERVER.GetClient(pPlayer->GetPair());

	//			DECLARE_JSON_WRITER
	//			ADD_JSON_MEMBER("Pos", pos)

	//				wstring Name = iter->second->m_Account.GetName();
	//			ADD_JSON_WSTR_MEMBER("PlayerName", Name)
	//				END_JSON_MEMBER(pPair->GetTCPSocket(), ID_FC_PKT_NEW_USER_IN_ROOM)

	//		}

	//		iter++;
	//		pos++;
	//	}

	//	RoomPtr->SendNewUserInfo(pPlayer);	//방에 있는 유저들에게 새로운 유저 정보전송
	//	return;
	//}

	//DECLARE_JSON_WRITER
	//ADD_JSON_MEMBER("Result", FC_PKT_ENTER_ROOM_RES::FAILED)
	//	END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_ENTER_ROOM_RES)
}

VOID BoardProcess::ROOM_LEAVE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pOwner)
{
	/*DECLARE_JSON_READ
	GET_JSON_UINT_MEMBER("RoomNumber", RoomNumber)

	PlayerPtr pPlayer = PLAYERMGR.Search(pOwner->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	ROOM_PTR RoomPtr = ROOMMGR.Search(RoomNumber);
	if (RoomPtr != NULL)
	{
		if (RoomPtr->FindPlayer(pPlayer) != USHRT_MAX)
		{
			FC_PKT_LEAVE_ROOM_RES res;

			res.Result = FC_PKT_LEAVE_ROOM_RES::SUCCESS;
			res.PlayerName = pPlayer->m_Account.GetName();

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result", FC_PKT_LEAVE_ROOM_RES::SUCCESS)
				ADD_JSON_WSTR_MEMBER("PlayerName", res.PlayerName)

				Json::FastWriter writer;
			std::string outputConfig = writer.write(root2);

			RoomPtr->SendToAll(ID_FC_PKT_LEAVE_ROOM_RES, (BYTE *)outputConfig.c_str(), outputConfig.size());

			RoomPtr->RemovePlayer(pPlayer);
			pPlayer->m_RoomNumber = 0;

			if (RoomPtr->GetCurrPlayer() == 0)
				ROOMMGR.Del(RoomPtr);
		}

		pPlayer->m_Char[0].SetAllComplete(FALSE);
		pPlayer->m_Char[0].SetReady(FALSE);

		return;

	}

	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result", FC_PKT_LEAVE_ROOM_RES::FAILED)
	ADD_JSON_WSTR_MEMBER("PlayerName", pPlayer->m_Account.GetName())
	END_JSON_MEMBER(pOwner->GetTCPSocket(), ID_FC_PKT_LEAVE_ROOM_RES)*/

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
	//DECLARE_JSON_READ
	//GET_JSON_WSTR_MEMBER("Chat", Chat)

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
	//		ADD_JSON_WSTR_MEMBER("Chat", Chat)

	//		Json::FastWriter writer;
	//		std::string outputConfig = writer.write(root2);

	//		pPtr->SendToAll(ID_FC_PKT_CHAT_ROOM_RES, (BYTE *)outputConfig.c_str(), outputConfig.size());
	//	}
	//}

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

	//	std::wstring Name = _T("AutoRoom");
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
