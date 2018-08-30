#include "StdAfx.h"
#include "GSJGPlayerProcess.h"

#include "../GSPacket/Front.h"
#include "../GSPacket/MainLogin.h"
#include "../PLAYER/Message/MSG_PLAYER_QUERY.h"
#include "../PLAYER/Container/Player.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../DBProxy/DBProcess.h"
#include "../GSPacket/Protocol.h"
#include "../room/RoomContainer.h"
#include "../Channel/ChannelContainer.h"
#include "GSSerialize.h"
#include "../LOG/FrontLog.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSMainProcess.h"
#include "GSAllocator.h"

using namespace GSFrontServer;

GSJGPlayerProcess::GSJGPlayerProcess(void)
{
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_CLOSE_REQ,CLOSE_PLAYER);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_REGIST_REQ,REGIST_PLAYER);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_CHOOSE_CHAR_REQ,CHOOSE_CHARACTER);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_ALL_COMPLETE_NTY,ALL_COMPLETE);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_AUTO_START_REQ,AUTO_START);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_ENTER_CHANNEL_REQ,ENTER_CHANNEL);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_USER_LIST_REQ,USER_LIST);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_ROOM_LIST_REQ,ROOM_LIST);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_CREATE_ROOM_REQ,ROOM_CREATE);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_ENTER_ROOM_REQ,ROOM_ENTER);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_LEAVE_ROOM_REQ,ROOM_LEAVE);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_CHAT_ROOM_REQ,ROOM_CHAT);
	ADD_NET_FUNC(GSJGPlayerProcess,ID_CF_PKT_LOBBY_CHAT_NTY,LOBBY_CHAT);

}

GSJGPlayerProcess::~GSJGPlayerProcess(void)
{
}


	VOID GSJGPlayerProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
	{
		NET_FUNC_EXE(MainProtocol,Data,Length);
	}

	VOID GSJGPlayerProcess::ACCESS_GAMESVR(LPVOID Data,DWORD Length)
	{
	}

	VOID GSJGPlayerProcess::REGIST_PLAYER(LPVOID Data,DWORD Length)
	{
		if(pOwner->GetPair() != ULONG_MAX)
			return ;

		if(pOwner->GetCompleteJob(_T("REGIST_PLAYER")) == true)		//이미 작업이진행중이다.
			return ;

		pOwner->SetCompleteJob(_T("REGIST_PLAYER"),true);

		DECLARE_JSON_READ
		GET_JSON_WSTR_MEMBER("Id",Id)
		GET_JSON_WSTR_MEMBER("SessionKey",SessionKey)

		RequestPlayerAuthPtr pRequest = ALLOCATOR.Create<RequestPlayerAuth>(); //(new RequestPlayerAuth);// 

		pRequest->Account    = Id.c_str();
		pRequest->SessionKey = SessionKey.c_str();

		pOwner->IncDBStamp();
		PROC_REG_QUERY_JOB(pRequest,DBP_GET_LOGIN_INFO,pOwner,GSFrontServer::MSG_PLAYER_QUERY,RequestPlayerAuth)
	}

	VOID GSJGPlayerProcess::CREATE_CHARACTER(LPVOID Data,DWORD Length)
	{
	}

	VOID GSJGPlayerProcess::LOBBY_CHAT(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_WSTR_MEMBER("Msg",Msg)

		CHANNEL_PTR Channel = CHANNELMGR.Search(0);
		if(Channel != NULL)
		{
			DECLARE_JSON_WRITER
			ADD_JSON_WSTR_MEMBER("Msg",Msg)
				Json::FastWriter writer;	
			std::string outputConfig = writer.write( root2 );	
			Channel->m_PlayerContainer.BroadCast(ID_CF_PKT_LOBBY_CHAT_NTY,0,(byte *)outputConfig.c_str(),outputConfig.size());
		}

	}

	VOID GSJGPlayerProcess::ENTER_CHANNEL(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_INT_MEMBER("ChannelId",ChannelId)

			CHANNEL_PTR Channel = CHANNELMGR.Search(ChannelId);
		if(Channel != NULL)
		{
			GSFrontServer::Player::PlayerPtr NewPlayer = PLAYERMGR.Search(pOwner->GetPair());
			if(NewPlayer != NULL)
			{
				Channel->m_PlayerContainer.Add(NewPlayer);
			}
			else
			{
				pOwner->Close();
				//pOwner->OnDisconnect();
			}

			Channel->m_PlayerContainer.Add(NewPlayer);
			NewPlayer->SetChannel(ChannelId);

			Channel->SendRoomList(pOwner);
			Channel->SendUserList(pOwner);

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("ChannelId",0)
			ADD_JSON_MEMBER("Result",FC_PKT_ENTER_CHANNEL_RES::SUCCESS)
			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_CF_PKT_ENTER_CHANNEL_RES)

			{
				DECLARE_JSON_WRITER
				END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_HELLO_WORLD)
			}

			return ;
		}

		printf("not found channel %d\n",ChannelId);

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("ChannelId",1)
			ADD_JSON_MEMBER("Result",FC_PKT_ENTER_CHANNEL_RES::FAILED)
			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_CF_PKT_ENTER_CHANNEL_RES)
	}

	VOID GSJGPlayerProcess::CHOOSE_CHARACTER(LPVOID Data,DWORD Length)
	{
		FLOG(_T("CALL CHOOSE CHARACTER\n"));

		DECLARE_JSON_READ
		GET_JSON_WSTR_MEMBER("CharName",CharName)

		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;


/*		GSCLIENT *pSession = UNITY.Search(1);
		if(pSession != NULL)
		{
			SESSION *ClientSession = pOwner;
			GSFrontServer::Player::PlayerPtr pPlayer = ChannelPtr->m_PlayerContainer.Search(ClientSession->GetPair());
			if(pPlayer != NULL)
			{
			}
			else	// 제대로 인증을 못한 유저다.
			{
		//		PROC_REG_CLOSE_JOB(pOwner,pOwner)
			}
		}
		else
		{
		//	PROC_REG_CLOSE_JOB(pOwner,pOwner)
		}*/

	}

	VOID GSJGPlayerProcess::CLOSE_PLAYER(LPVOID Data,DWORD Length)
	{

		pOwner->GetTCPSocket()->Termination();
	}

	VOID GSJGPlayerProcess::USER_LIST(LPVOID Data,DWORD Length)
	{
		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		ChannelPtr->SendUserList(pOwner);

	}

	VOID GSJGPlayerProcess::ROOM_LIST(LPVOID Data,DWORD Length)
	{

		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		ChannelPtr->SendRoomList(pOwner);
	}

	VOID GSJGPlayerProcess::ROOM_CREATE(LPVOID Data,DWORD Length)
	{
		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",FC_PKT_CREATE_ROOM_RES::ErrorCode::NOT_FOUND_PLAYER)
				END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_CREATE_ROOM_RES)
				return ;
		}

		DECLARE_JSON_READ
		GET_JSON_WSTR_MEMBER("Name",Name)
			GET_JSON_INT_MEMBER("Type",Type)
			GET_JSON_INT_MEMBER("MaxCount",MaxCount)

			ROOM_PTR RoomPtr = ChannelPtr->m_RoomContainer.Create();
		RoomPtr->m_Stock.Name = Name;

		if(RoomPtr->CreateGameRule(Type) == FALSE)
		{
			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",FC_PKT_CREATE_ROOM_RES::ErrorCode::NOT_EXIST_GAME_TYPE)
				END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_CREATE_ROOM_RES)
				return ;
		}

		ChannelPtr->m_RoomContainer.Add(RoomPtr);
		RoomPtr->m_Stock.MAX_PLAYER = MaxCount;

		if(pPlayer != NULL)
		{
			pPlayer->m_RoomNumber = RoomPtr->GetId();
		}

		RoomPtr->InsertPlayer(pPlayer);

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Result",0)
			ADD_JSON_MEMBER("RoomNumber",(WORD)RoomPtr->GetId())
			ADD_JSON_MEMBER("MaxCount",RoomPtr->m_Stock.MAX_PLAYER)
			ADD_JSON_WSTR_MEMBER("Name",RoomPtr->m_Stock.Name)
			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_CREATE_ROOM_RES)

			RoomPtr->SendNewUserInfo(pPlayer);	//방에 있는 유저들에게 새로운 유저 정보전송
	}

	VOID GSJGPlayerProcess::ROOM_ENTER(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_UINT_MEMBER("RoomNumber",RoomNumber)

			CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		ROOM_PTR RoomPtr = ChannelPtr->m_RoomContainer.Search(RoomNumber);
		if(RoomPtr != NULL)
		{
			GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
			if(pPlayer != NULL)
			{
				RoomPtr->InsertPlayer(pPlayer);

				pPlayer->m_RoomNumber = RoomPtr->GetId();
			}

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",0)

				FC_PKT_ENTER_ROOM_RES res;
			res.Result = FC_PKT_ENTER_ROOM_RES::SUCCESS;
			int pos = 0;
			res.roominfo.Index = RoomPtr->GetId();
			res.roominfo.Name = RoomPtr->m_Stock.Name;

			root2["roominfo"]["Index"] = (WORD)RoomPtr->GetId();
			root2["roominfo"]["Name"]  = RoomPtr->m_Stock.Name.c_str();

			Json::Value Array;
			res.Result = 0;
			root2["PlayerList"] = Array;

			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_ENTER_ROOM_RES)

				std::map<DWORD,PLAYER_PTR>::iterator iter = RoomPtr->m_PlayerMap.begin();	
			while(iter != RoomPtr->m_PlayerMap.end())
			{
				Array["Pos"]		= (WORD)iter->first;
				root2["PlayerName"] = iter->second->m_Account.GetName().c_str();

				DECLARE_JSON_WRITER
				ADD_JSON_MEMBER("Pos",pos)

					wstring Name = iter->second->m_Account.GetName();
				ADD_JSON_WSTR_MEMBER("PlayerName",Name)
					END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_NEW_USER_IN_ROOM)

					res.Pos[pos] = iter->first;
				res.PlayerName[pos] = iter->second->m_Account.GetName();

				iter++;
				pos++;
			}




			iter = RoomPtr->m_PlayerMap.begin();	
			while(iter != RoomPtr->m_PlayerMap.end())
			{
				//방안의 유저들 정보를 새로운 유저에게 전송

				if(iter->second->m_Account.GetName() != pPlayer->m_Account.GetName())
				{
					GSCLIENT *pPair =JGSERVER.GetClient(pPlayer->GetPair());

					DECLARE_JSON_WRITER
					ADD_JSON_MEMBER("Pos",pos)

					wstring Name = iter->second->m_Account.GetName();
					ADD_JSON_WSTR_MEMBER("PlayerName",Name)
					END_JSON_MEMBER(pPair->GetTCPSocket(),ID_FC_PKT_NEW_USER_IN_ROOM)

				}

				iter++;
				pos++;
			}

			RoomPtr->SendNewUserInfo(pPlayer);	//방에 있는 유저들에게 새로운 유저 정보전송
			return ;
		}

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Result",FC_PKT_ENTER_ROOM_RES::FAILED)
			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_ENTER_ROOM_RES)
	}

	VOID GSJGPlayerProcess::ROOM_LEAVE(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_UINT_MEMBER("RoomNumber",RoomNumber)

			CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		ROOM_PTR RoomPtr = ChannelPtr->m_RoomContainer.Search(RoomNumber);
		if(RoomPtr != NULL)
		{
			GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
			if(pPlayer != NULL)
			{
				if(RoomPtr->FindPlayer(pPlayer) != USHRT_MAX)
				{
					FC_PKT_LEAVE_ROOM_RES res;

					res.Result = FC_PKT_LEAVE_ROOM_RES::SUCCESS;
					res.PlayerName = pPlayer->m_Account.GetName();

					DECLARE_JSON_WRITER
					ADD_JSON_MEMBER("Result",FC_PKT_LEAVE_ROOM_RES::SUCCESS)
						ADD_JSON_WSTR_MEMBER("PlayerName",res.PlayerName)

						Json::FastWriter writer;	
					std::string outputConfig = writer.write( root2 );	

					RoomPtr->SendToAll(ID_FC_PKT_LEAVE_ROOM_RES,(BYTE *)outputConfig.c_str(),outputConfig.size());

					RoomPtr->RemovePlayer(pPlayer);
					pPlayer->m_RoomNumber = 0;

					if(RoomPtr->GetCurrPlayer() == 0)
						ChannelPtr->m_RoomContainer.Del(RoomPtr);
				}

				pPlayer->m_Char[0].SetAllComplete(FALSE);
				pPlayer->m_Char[0].SetReady(FALSE);

				return ;
			}
		}

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Result",FC_PKT_LEAVE_ROOM_RES::FAILED)
			ADD_JSON_WSTR_MEMBER("PlayerName",pPlayer->m_Account.GetName())
			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FC_PKT_LEAVE_ROOM_RES)

	}

	VOID GSJGPlayerProcess::ROOM_START(LPVOID Data,DWORD Length)
	{

		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		if(pPlayer != NULL)
		{
			ROOM_PTR pPtr = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
			if(pPtr != NULL)
			{
				DECLARE_JSON_WRITER
				Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	

				pPtr->SendToAll(ID_CF_PKT_START_ROOM_REQ,(BYTE *)outputConfig.c_str(),outputConfig.size());
			}
		}
	}

	VOID GSJGPlayerProcess::ROOM_READY(LPVOID Data,DWORD Length)
	{
		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		if(pPlayer != NULL)
		{
			ROOM_PTR pPtr = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
			if(pPtr != NULL)
			{
				if(pPlayer->m_Char[0].GetReady() == FALSE)
					pPlayer->m_Char[0].SetReady(TRUE);
				else 
					pPlayer->m_Char[0].SetReady(FALSE);

				FC_PKT_READY_ROOM_RES snd;
				snd.Name   = pPlayer->m_Char[0].GetName();
				snd.Result = pPlayer->m_Char[0].GetReady();

				DECLARE_JSON_WRITER
				ADD_JSON_WSTR_MEMBER("Name",pPlayer->m_Char[0].GetName())
					ADD_JSON_MEMBER("Result",pPlayer->m_Char[0].GetReady())

					Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	

				pPtr->SendToAll(ID_FC_PKT_READY_ROOM_RES,(BYTE *)outputConfig.c_str(),outputConfig.size());
			}
		}

	}

	VOID GSJGPlayerProcess::ROOM_CHAT(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_WSTR_MEMBER("Chat",Chat)

			CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		if(pPlayer != NULL)
		{
			ROOM_PTR pPtr = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
			if(pPtr != NULL)
			{
				DECLARE_JSON_WRITER
				ADD_JSON_WSTR_MEMBER("Chat",Chat)

					Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	

				pPtr->SendToAll(ID_FC_PKT_CHAT_ROOM_RES,(BYTE *)outputConfig.c_str(),outputConfig.size());
			}
		}

	}

	VOID GSJGPlayerProcess::AUTO_START(LPVOID Data,DWORD Length)
	{
		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		DECLARE_JSON_READ
		GET_JSON_UINT_MEMBER("GameType",GameType)

			ROOM_PTR RoomPtr = ChannelPtr->m_RoomContainer.SearchByGameType(GameType);
		if(RoomPtr ==  NULL)
		{
			DECLARE_JSON_WRITER

			std::wstring Name = _T("AutoRoom");
			ADD_JSON_WSTR_MEMBER("Name",Name)

				ADD_JSON_MEMBER("MaxCount",2)
				ADD_JSON_MEMBER("Type",GameType)
				Json::FastWriter writer;	
			std::string outputConfig = writer.write( root2 );	

			ROOM_CREATE((char*)outputConfig.c_str(),outputConfig.size());
		}
		else
		{
			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("RoomNumber",(WORD)RoomPtr->GetId())
				Json::FastWriter writer;	
			std::string outputConfig = writer.write( root2 );	
			ROOM_ENTER((char*)outputConfig.c_str(),outputConfig.size());
		}

	}

	VOID GSJGPlayerProcess::ALL_COMPLETE(LPVOID Data,DWORD Length)
	{
		CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;

		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		if(pPlayer != NULL)
		{
			pPlayer->m_Char[0].SetAllComplete(TRUE);
		}
		else
			return ;


		ROOM_PTR pPtr = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
		if(pPtr != NULL)
		{
			if(pPtr->IsAllComplete() == true)
			{
				DECLARE_JSON_WRITER

				Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	

				pPtr->SendToAll(ID_FC_PKT_ALL_COMPLETE,(BYTE *)outputConfig.c_str(),outputConfig.size());
			}
		}
	}

	VOID GSJGPlayerProcess::Move(LPVOID Data,DWORD Length)
	{
		DECLARE_JSON_READ
		GET_JSON_UINT_MEMBER("x",x)
			GET_JSON_UINT_MEMBER("y",y)
			GET_JSON_UINT_MEMBER("z",z)

			CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
		if(ChannelPtr == NULL)
			return ;


		GSFrontServer::Player::PlayerPtr pPlayer= ChannelPtr->m_PlayerContainer.Search(pOwner->GetPair());
		if(pPlayer == NULL)
		{
			return ;
		}

		pPlayer->m_Char[0].SetAllComplete(TRUE);

		ROOM_PTR pPtr = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
		if(pPtr != NULL)
		{
			if(pPtr->IsAllComplete() == true)
			{

				DECLARE_JSON_WRITER
				ADD_JSON_MEMBER("x",x)
					ADD_JSON_MEMBER("y",y)
					ADD_JSON_MEMBER("z",z)

					Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	

				pPtr->SendToAll(ID_FC_PKT_MOVE_RES,(BYTE *)outputConfig.c_str(),outputConfig.size());
			}
		}

	}