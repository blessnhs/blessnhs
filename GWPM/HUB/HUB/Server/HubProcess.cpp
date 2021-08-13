#include "StdAfx.h"

#include "HubProcess.h"
#include "Base64.h"

using namespace google;

HubProcess::HubProcess(void)
{
	ADD_NET_FUNC(HubProcess, ID_PKT_VERSION_REQ, VERSION);
	ADD_NET_FUNC(HubProcess, ID_PKT_LOGIN_REQ, LOGIN_PLAYER);
	ADD_NET_FUNC(HubProcess, ID_PKT_CREATE_ROOM_REQ, ROOM_CREATE);
	ADD_NET_FUNC(HubProcess, ID_PKT_ENTER_ROOM_REQ, ROOM_ENTER);
	ADD_NET_FUNC(HubProcess, ID_PKT_LEAVE_ROOM_REQ, ROOM_LEAVE);
	ADD_NET_FUNC(HubProcess, ID_PKT_BITMAP_MESSAGE_REQ, ROOM_BITMAP_CHAT);
	ADD_NET_FUNC(HubProcess, ID_PKT_ROOM_LIST_REQ, ROOM_LIST);
	ADD_NET_FUNC(HubProcess, ID_PKT_ROOM_PASS_THROUGH_REQ, ROOM_PASSTHROUGH);
	ADD_NET_FUNC(HubProcess, ID_PKT_NOTICE_REQ, NOTICE);
	ADD_NET_FUNC(HubProcess, ID_PKT_AUDIO_MESSAGE_REQ, ROOM_AUDIO_CHAT);

	ADD_NET_FUNC(HubProcess, ID_PKT_PRAY_MESSAGE_REQ, PRAY_LIST);
	ADD_NET_FUNC(HubProcess, ID_PKT_PRAY_MESSAGE_REG_REQ, REG_PRAY);

	ADD_NET_FUNC(HubProcess, ID_PKT_QNA_REQ, QNS);
}


HubProcess::~HubProcess(void)
{
}


VOID HubProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client)
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

		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();
		std::string name = descriptor->FindValueByNumber(MainProtocol)->name();

		if(MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
			BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);

		NET_FUNC_EXE(MainProtocol, Data, Length, Client);
	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

VOID HubProcess::CHECK_NICKNAME(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
}

VOID HubProcess::NOTICE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(NOTICE_REQ, version)	
	
	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestNotice>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestNotice>>();
	PLAYER_MSG->pSession = Client;
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::VERSION(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(VERSION_REQ, version)

	VERSION_RES res;

	res.set_var_code(Success);
	res.set_var_version(INI.Version);

	SEND_PROTO_BUFFER(res, Client)

		////버전 쿼리를 날린다.
		//boost::shared_ptr<RequestVersion> pRequest = ALLOCATOR.Create<RequestVersion>();
		//
		//boost::shared_ptr<Hub::MSG_PLAYER_QUERY<RequestVersion>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<RequestVersion>>();
		//PLAYER_MSG->pSession = Client;
		//PLAYER_MSG->pRequst = pRequest;
		//PLAYER_MSG->Type = Client->GetMyDBTP();
		//PLAYER_MSG->SubType = ONQUERY;
		//MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::LOGIN_PLAYER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(LOGIN_REQ, login)

	if (_result_, login.var_id().size() <= 0 || login.var_pwd().size() <= 0)
		return;

	if (_result_, login.var_id().size() >= 256 || login.var_pwd().size() >= 256)
		return;

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<RequestPlayerAuth>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<RequestPlayerAuth>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->pRequst.id.assign(login.var_id().begin(), login.var_id().end());
		PLAYER_MSG->pRequst.pwd.assign(login.var_pwd().begin(), login.var_pwd().end());
		PLAYER_MSG->pRequst.channel = 1;// login.var_channel();
	}

	//로그인아웃 처리는 고정해야할 필요가 있다.
	//id에 의해 분할되면 멀티 쓰레드 동기화 문제가 발생할 가능성이 존재한다.
	PLAYER_MSG->Type = MSG_TYPE_DB_1;
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::ROOM_CREATE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(CREATE_ROOM_REQ, createroom)

	CREATE_ROOM_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client)
			return;
	}

	if (createroom.var_name().size() == 0)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client)
		return;
	}

	//디비에 먼저 기록을 남긴다.

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::CreateRooom>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::CreateRooom>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string, INT64, string, PlayerPtr>(createroom.var_name(), pPlayer->GetId(), pPlayer->m_Char[0].GetName(), pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::PRAY_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(PRAY_MESSAGE_REQ, message)

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestPrayList>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestPrayList>>();
	PLAYER_MSG->pSession = Client;

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::REG_PRAY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(PRAY_MESSAGE_REG_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}
	
	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestRegPray>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestRegPray>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string,string>(message.var_message(), pPlayer->m_Char[0].GetName());
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::QNS(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(QNA_REQ, message)

	if (message.var_message().length() > 2048 || message.var_message().length() <= 0)
	{
		return;
	}

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestQNS>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestQNS>>();
	{
		PLAYER_MSG->Request.m_args = std::tuple<string, INT64>(message.var_message(), pPlayer->GetId());
	}
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	PLAYER_MSG->pSession = Client;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::ROOM_PASSTHROUGH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ROOM_PASS_THROUGH_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	if (message.var_message().length() > 1024 || message.var_message().length() <= 0)
	{
		return;
	}

	ROOM_PTR RoomPtr = ROOMMGR.Search(message.var_room_number());
	if (RoomPtr == NULL)
	{
		return;
	}

	if (RoomPtr->FindPlayer(pPlayer) == FALSE)
	{
		return;
	}

	/*m_MessageList.push_back()*/

	ROOM_PASS_THROUGH_RES res;
	auto pass_msg = res.add_var_messages();
	pass_msg->set_var_message(message.var_message());
	pass_msg->set_var_message_int(message.var_message_int());
	pass_msg->set_var_name(pPlayer->m_Char[0].GetName());
	pass_msg->set_var_time(message.var_time());

	res.set_var_room_number(RoomPtr->GetId());
	RoomPtr->SendToAll(res);

	RoomPtr->AddRoomMessage(*pass_msg);

	//디비에 먼저 기록을 남긴다.

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RoomPassThrou>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RoomPassThrou>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string, string>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->m_Char[0].GetName(), message.var_message());
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

	
}

VOID HubProcess::RANK(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
}

VOID HubProcess::ROOM_ENTER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ENTER_ROOM_REQ, enterroom)

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	ENTER_ROOM_RES res;


	ROOM_PTR RoomPtr = ROOMMGR.Search(enterroom.var_room_number());
	if (RoomPtr == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client)
		return;
	}

	//이미 입장 해 있다면 
	if (RoomPtr->FindPlayer(pPlayer) == TRUE)
	{
		res.set_var_room_id(RoomPtr->GetId());
		res.set_var_name(RoomPtr->m_Stock.Name.c_str());
		res.set_var_code(Duplicate_Enter_Room);
		SEND_PROTO_BUFFER(res, Client)
			return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::EnterRooom>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::EnterRooom>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->m_Char[0].GetName());
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

	
}

VOID HubProcess::ROOM_LEAVE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(LEAVE_ROOM_REQ, leaveroom)

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	ROOM_PTR RoomPtr = ROOMMGR.Search(leaveroom.var_room_number());
	if (RoomPtr == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::LeaveRoom>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::LeaveRoom>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->m_Char[0].GetName());
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::ROOM_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
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

VOID HubProcess::ROOM_READY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
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

VOID HubProcess::ROOM_AUDIO_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(AUDIO_MESSAGE_REQ, message)

	AUDIO_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}


	for each (auto msg  in message.var_message())
	{
		res.add_var_message(msg);
	}

	res.set_var_name(pPlayer->m_Account.GetName());

	ROOM_PTR pPtr = ROOMMGR.Search(message.var_room_number());
	if (pPtr != NULL)
	{
		pPtr->SendToAll(res, pPlayer->GetId());
	}
	else
	{
		SEND_PROTO_BUFFER(res, Client)
	}
}

VOID HubProcess::ROOM_BITMAP_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(BITMAP_MESSAGE_REQ, message)

	BITMAP_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	for each (auto msg  in message.var_message())
	{
		res.add_var_message(msg);
	}

	res.set_var_name(pPlayer->m_Account.GetName());

	ROOM_PTR pPtr = ROOMMGR.Search(message.var_room_number());
	if (pPtr != NULL)
	{
		pPtr->SendToAll(res, pPlayer->GetId());
	}
	else
	{
		SEND_PROTO_BUFFER(res, Client)
	}
}

VOID HubProcess::ROOM_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ROOM_LIST_REQ, roomlistreq)

	ROOM_LIST_RES res;

	PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
	if (pPlayer == NULL)
	{
		return;
	}

	ROOMMGR.GetRoomList(res.mutable_var_room_list());

	SEND_PROTO_BUFFER(res, Client)
}

VOID HubProcess::MATCH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
}


VOID HubProcess::AUTO_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
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

	//	ROOM_CREATE((char*)outputConfig.c_str(), outputConfig.size(), Client);
	//}
	//else
	//{
	//	DECLARE_JSON_WRITER
	//	ADD_JSON_MEMBER("RoomNumber", (WORD)RoomPtr->GetId())
	//		Json::FastWriter writer;
	//	std::string outputConfig = writer.write(root2);
	//	ROOM_ENTER((char*)outputConfig.c_str(), outputConfig.size(), Client);
	//}

}

VOID HubProcess::ALL_COMPLETE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.Search(Client->GetPair());
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
