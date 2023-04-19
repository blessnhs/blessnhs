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

	ADD_NET_FUNC(HubProcess, ID_PKT_CLIENT_LOGOUT_REQ, LOGOUT_CLIENT);

	ADD_NET_FUNC(HubProcess, ID_PKT_MPEG2TS_MESSAGE_REQ, MPEG2TS_MESSAGE);

	ADD_NET_FUNC(HubProcess, ID_PKT_CAMERA_WAKE_UP_REQ, CAMERA_WAKE_UP);

	ADD_NET_FUNC(HubProcess, ID_PKT_REG_CAMERA_REQ, CAMERA_REGISTER);

	ADD_NET_FUNC(HubProcess, ID_PKT_CAMERA_LIST_REQ, CAMERA_LIST);

	ADD_NET_FUNC(HubProcess, ID_PKT_STOP_STREAM_REQ, STOP_STREAM);

	ADD_NET_FUNC(HubProcess, ID_PKT_CAMERA_CONTROL_REQ, CAMERA_CONTROL);

	ADD_NET_FUNC(HubProcess, ID_PKT_MACHINE_STATUS_REQ, MACHINE_STATUS);

	ADD_NET_FUNC(HubProcess, ID_PKT_VERIFY_PURCHASE_REQ, PURCHASE_VERIFY);
	ADD_NET_FUNC(HubProcess, ID_PKT_DEL_CAMERA_REQ, CAMERA_DELETE);

	ADD_NET_FUNC(HubProcess, ID_PKT_RECONNECT_REQ, RECONNECT);

	
	
	

}


HubProcess::~HubProcess(void)
{
}


VOID HubProcess::Process(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	try
	{

		PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
		if (pPlayer != NULL)
		{
			pPlayer->m_AliveTime = GetTickCount();
		}

		//로그인 하지 않은 유저가 패킷을 요청 했을때
		// 버전이나 로그인 패킷이 아닌 경우 처리하지 않는다.
		if (pBuffer->MainId != ID_PKT_VERSION_REQ && pBuffer->MainId != ID_PKT_LOGIN_REQ)
		{
			if (pPlayer == NULL)
			{
#ifdef _DEBUG
#else
				//	return;
#endif
			}
		}

#ifdef _DEBUG
		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();

		std::string name;
		if(descriptor->FindValueByNumber(pBuffer->MainId) != NULL)
			name = descriptor->FindValueByNumber(pBuffer->MainId)->name();

	/*	if (pBuffer->MainId != ID_PKT_ROOM_LIST_REQ && pBuffer->MainId != ID_PKT_PRAY_MESSAGE_REQ)
			BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), pBuffer->Length);*/
#endif

		NET_FUNC_EXE2(pBuffer, Client);
	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

VOID HubProcess::CAMERA_DELETE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(DEL_CAMERA_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::DelCamera>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::DelCamera>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string, PlayerPtr>(message.var_machine_id(), pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::CHECK_NICKNAME(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
}

VOID HubProcess::NOTICE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}
	DECLARE_RECV_TYPE(NOTICE_REQ, version)

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestNotice>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestNotice>>();
	PLAYER_MSG->pSession = Client;
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;

	{
		PLAYER_MSG->Request.m_args = std::tuple<PlayerPtr>(pPlayer);;
	}

	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::VERSION(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(VERSION_REQ, version)

	VERSION_RES res;

	res.set_var_code(Success);
	res.set_var_version(INI.Version);

	SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)

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

VOID HubProcess::LOGIN_PLAYER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(LOGIN_REQ, login)

	if (_result_, login.var_token().size() <= 0)
		return;

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<RequestPlayerAuth>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<RequestPlayerAuth>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->pRequst.token.assign(login.var_token().begin(), login.var_token().end());
		PLAYER_MSG->pRequst.channel = 1;// login.var_channel();
		PLAYER_MSG->pRequst.ForntId = Client->GetId();
		PLAYER_MSG->pRequst.FrontSid = pBuffer->Reserve2;
		PLAYER_MSG->pRequst.MachineId = login.var_machine_id();
		PLAYER_MSG->pRequst.MachineModel = login.var_cam_name();
		PLAYER_MSG->pRequst.Ip = login.var_ip();
	}

	//로그인아웃 처리는 고정해야할 필요가 있다.
	//id에 의해 분할되면 멀티 쓰레드 동기화 문제가 발생할 가능성이 존재한다.
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::ROOM_CREATE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(CREATE_ROOM_REQ, createroom)

	CREATE_ROOM_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
			return;
	}

	if (createroom.var_name().size() == 0)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
			return;
	}

	//디비에 먼저 기록을 남긴다.

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::CreateRooom>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::CreateRooom>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string, INT64, string, PlayerPtr>(createroom.var_name(), pPlayer->GetId(), pPlayer->GetName(), pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::PRAY_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	DECLARE_RECV_TYPE(PRAY_MESSAGE_REQ, message)

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestPrayList>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestPrayList>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<PlayerPtr>(pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::REG_PRAY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(PRAY_MESSAGE_REG_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestRegPray>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestRegPray>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string, string, PlayerPtr>(message.var_message(), pPlayer->GetName(), pPlayer);
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::QNS(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(QNA_REQ, message)

	if (message.var_message().length() > 2048 || message.var_message().length() <= 0)
	{
		return;
	}

	INT64 PID = 0;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		PID = 0;
	}
	else
		PID = pPlayer->GetId();

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestQNS>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestQNS>>();
	{
		PLAYER_MSG->Request.m_args = std::tuple<string, INT64, PlayerPtr>(message.var_message(), PID, pPlayer);
	}
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	PLAYER_MSG->pSession = Client;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::ROOM_PASSTHROUGH(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ROOM_PASS_THROUGH_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
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
	pass_msg->set_var_name(pPlayer->GetName());
	pass_msg->set_var_time(message.var_time());

	res.set_var_room_number(RoomPtr->GetId());
	RoomPtr->SendToAll(res);

	RoomPtr->AddRoomMessage(*pass_msg);

	//디비에 먼저 기록을 남긴다.

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RoomPassThrou>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RoomPassThrou>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string, string>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->GetName(), message.var_message());
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);


}

VOID HubProcess::RANK(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
}

VOID HubProcess::ROOM_ENTER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ENTER_ROOM_REQ, enterroom)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	ENTER_ROOM_RES res;


	ROOM_PTR RoomPtr = ROOMMGR.Search(enterroom.var_room_number());
	if (RoomPtr == NULL)
	{
		res.set_var_code(SystemError);
		SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
			return;
	}

	//이미 입장 해 있다면 
	if (RoomPtr->FindPlayer(pPlayer) == TRUE)
	{
		res.set_var_room_id(RoomPtr->GetId());
		res.set_var_name(RoomPtr->m_Stock.Name.c_str());
		res.set_var_code(Duplicate_Enter_Room);
		SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
			return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::EnterRooom>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::EnterRooom>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string, PlayerPtr>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->GetName(), pPlayer);
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);


}

VOID HubProcess::ROOM_LEAVE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(LEAVE_ROOM_REQ, leaveroom)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
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
		PLAYER_MSG->Request.m_args = std::tuple<int, INT64, string, PlayerPtr>(RoomPtr->GetId(), pPlayer->GetId(), pPlayer->GetName(), pPlayer);
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);

}

VOID HubProcess::ROOM_START(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
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

VOID HubProcess::ROOM_READY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//if (pPlayer != NULL)
	//{
	//	ROOM_PTR pPtr = ROOMMGR.Search(pPlayer->m_RoomNumber);
	//	if (pPtr != NULL)
	//	{
	//		if (pPlayer->GetReady() == FALSE)
	//			pPlayer->SetReady(TRUE);
	//		else
	//			pPlayer->SetReady(FALSE);

	//		FC_PKT_READY_ROOM_RES snd;
	//		snd.Name = pPlayer->GetName();
	//		snd.Result = pPlayer->GetReady();

	//		DECLARE_JSON_WRITER
	//		ADD_JSON_WSTR_MEMBER("Name", pPlayer->GetName())
	//			ADD_JSON_MEMBER("Result", pPlayer->GetReady())

	//			Json::FastWriter writer;
	//		std::string outputConfig = writer.write(root2);

	//		pPtr->SendToAll(ID_FC_PKT_READY_ROOM_RES, (BYTE *)outputConfig.c_str(), outputConfig.size());
	//	}
	//}

}

VOID HubProcess::ROOM_AUDIO_CHAT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(AUDIO_MESSAGE_REQ, message)

	AUDIO_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}


	for each (auto msg  in message.var_message())
	{
		res.add_var_message(msg);
	}

	res.set_var_name(pPlayer->m_Account.GetName());

	for each (auto pid in message.var_to_player_id())
	{
		PlayerPtr pPlayerTarget = PLAYERMGR.Search(pid);
		if (pPlayerTarget != NULL)
		{
			GSCLIENT_PTR pSession = SERVER.GetClient(pPlayerTarget->GetPair());
			if (pSession)
				SEND_PROTO_BUFFER(res, pSession, pPlayerTarget->GetFrontSid())
		}
	}
}

VOID HubProcess::ROOM_BITMAP_CHAT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(BITMAP_MESSAGE_REQ, message)

	BITMAP_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	res.set_var_name(pPlayer->GetMachineName());
	res.set_var_width(message.var_width());
	res.set_var_height(message.var_height());


	for each (auto msg  in message.var_message())
	{
		res.add_var_message(msg);
	}

	for each (auto pid in message.var_to_player_id())
	{
		PlayerPtr pPlayerTarget = PLAYERMGR.Search(pid);
		if (pPlayerTarget != NULL)
		{
			GSCLIENT_PTR pSession = SERVER.GetClient(pPlayerTarget->GetPair());
			if (pSession)
				SEND_PROTO_BUFFER(res, pSession, pPlayerTarget->GetFrontSid())
		}
		else
		{
			//없으면 중지 요청
			STOP_STREAM_RES res;
			res.set_var_code(Success);
			res.set_var_to_player_id(pid);
			SEND_PROTO_BUFFER(res, Client, pPlayer->GetFrontSid())
		}
	}
}


VOID HubProcess::MPEG2TS_MESSAGE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(MPEG2TS_MESSAGE_REQ, message)

	MPEG2TS_MESSAGE_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	for each (auto msg  in message.var_message())
	{
		res.add_var_message(msg);
	}


	res.set_var_type(message.var_type());

	ROOM_PTR pPtr = ROOMMGR.Search(message.var_room_number());
	if (pPtr != NULL)
	{
		int pos = pPtr->FindPlayerPos(pPlayer);

		pPtr->SendToAll(res, /*pPlayer->GetId()*/-1);
	}
	else
	{
		SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
	}
}

VOID HubProcess::CAMERA_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(CAMERA_LIST_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::CameraList>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::CameraList>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple< PlayerPtr>(pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::CAMERA_REGISTER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(REG_CAMERA_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RegCamera>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RegCamera>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->Request.m_args = std::tuple<string, string, PlayerPtr>(message.var_machine_id(), message.var_cam_name(), pPlayer);;
	}

	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::CAMERA_CONTROL(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(CAMERA_CONTROL_REQ, message)

	CAMERA_CONTROL_RES res;
	res.set_var_code(Success);


	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	PlayerPtr pTargetPlayer = PLAYERMGR.Search(message.var_player_id());
	if (pTargetPlayer == NULL)
	{
		return;
	}

	res.set_var_type(message.var_type());

	res.set_var_machine_id(message.var_machine_id());
	res.set_var_player_id(message.var_player_id());

	GSCLIENT_PTR pSession = SERVER.GetClient(pTargetPlayer->GetPair());
	if (pSession)
	{
		SEND_PROTO_BUFFER(res, pSession, pTargetPlayer->GetFrontSid())
	}
}

VOID HubProcess::CAMERA_WAKE_UP(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(CAMERA_WAKE_UP_REQ, message)

	CAMERA_WAKE_UP_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	PlayerPtr pTargetPlayer = PLAYERMGR.Search(message.var_to_player_id());
	if (pTargetPlayer == NULL)
	{
		return;
	}

	res.set_var_machine_id(message.var_machine_id());
	res.set_var_to_player_id(pPlayer->GetId());

	res.set_var_type(message.var_type());


	//요청을 받은 대상
	GSCLIENT_PTR pSession = SERVER.GetClient(pTargetPlayer->GetPair());
	if (pSession)
	{
		SEND_PROTO_BUFFER(res, pSession, pTargetPlayer->GetFrontSid())
	}


	//이번엔 바꾼다.
	res.set_var_machine_id(message.var_machine_id());
	res.set_var_to_player_id(pTargetPlayer->GetId());

	//요청한 대상 창을 띄울 타이밍
	SEND_PROTO_BUFFER(res, Client, pPlayer->GetFrontSid())
}


VOID HubProcess::ROOM_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(ROOM_LIST_REQ, roomlistreq)

	ROOM_LIST_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	ROOMMGR.GetRoomList(res.mutable_var_room_list());

	SEND_PROTO_BUFFER(res, Client, pBuffer->Reserve2)
}

VOID HubProcess::MATCH(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
}

VOID HubProcess::STOP_STREAM(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(STOP_STREAM_REQ, message)


	STOP_STREAM_RES res;

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
	{
		return;
	}

	res.set_var_code(Success);
	res.set_var_to_player_id(pPlayer->GetId());

	PLAYERMGR.BroadCast(res, pPlayer->GetDBIndex());


}


VOID HubProcess::AUTO_START(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
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

VOID HubProcess::ALL_COMPLETE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	//PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	//if (pPlayer == NULL)
	//{
	//	return;
	//}

	//pPlayer->SetAllComplete(TRUE);

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

VOID HubProcess::PURCHASE_VERIFY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	DECLARE_RECV_TYPE(VERIFY_PURCHASE_REQ, message)

	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer == NULL)
		return;

	boost::shared_ptr<Hub::MSG_PLAYER_QUERY<RequestVerifyPurchase>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<RequestVerifyPurchase>>();
	PLAYER_MSG->pSession = Client;

	{
		PLAYER_MSG->pRequst.token.assign(message.var_token().begin(), message.var_token().end());
		PLAYER_MSG->pRequst.PacketId.assign(message.var_package_name().begin(), message.var_package_name().end());
		PLAYER_MSG->pRequst.PurchaseId.assign(message.var_purchase_id().begin(), message.var_purchase_id().end());
		PLAYER_MSG->pRequst.ForntId = Client->GetId();
		PLAYER_MSG->pRequst.FrontSid = pBuffer->Reserve2;
		PLAYER_MSG->pRequst.pPlayer = pPlayer;
	}

	//로그인아웃 처리는 고정해야할 필요가 있다.
	//id에 의해 분할되면 멀티 쓰레드 동기화 문제가 발생할 가능성이 존재한다.
	PLAYER_MSG->Type = Client->GetMyDBTP();
	PLAYER_MSG->SubType = ONQUERY;
	MAINPROC.RegisterCommand(PLAYER_MSG);
}

VOID HubProcess::LOGOUT_CLIENT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
	if (pPlayer != NULL)
	{

		//로그아웃 쿼리를 날린다.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::RequestLogout>>();
		PLAYER_MSG->pSession = Client;

		PLAYER_MSG->Request.m_args = std::tuple<INT64>(pPlayer->GetId());

		PLAYER_MSG->Type = Client->GetMyDBTP();
		PLAYER_MSG->SubType = ONQUERY;
		MAINPROC.RegisterCommand(PLAYER_MSG);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		for each (auto room in pPlayer->GetRoom())
		{
			ROOMMGR.LeaveRoomPlayer(pPlayer, room);
		}


		//		pPlayer->SetPair(ULONG_MAX);
		PLAYERMGR.Del(pPlayer);
	}

}
	VOID HubProcess::MACHINE_STATUS(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
	{
		DECLARE_RECV_TYPE(MACHINE_STATUS_REQ, message)

		MACHINE_STATUS_RES res;

		PlayerPtr pPlayer = PLAYERMGR.SearchByFrontSid(pBuffer->Reserve2);
		if (pPlayer == NULL)
		{
			return;
		}

		res.set_var_battery( message.var_battery());

		for each (auto pid in message.var_to_player_id())
		{
			PlayerPtr pPlayerTarget = PLAYERMGR.Search(pid);
			if (pPlayerTarget != NULL)
			{
				GSCLIENT_PTR pSession = SERVER.GetClient(pPlayerTarget->GetPair());
				if (pSession)
					SEND_PROTO_BUFFER(res, pSession, pPlayerTarget->GetFrontSid())
			}
		}
	}


	VOID HubProcess::RECONNECT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
	{
		DECLARE_RECV_TYPE(RECONNECT_REQ, message)

	

		boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::Reconnect>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::Reconnect>>();
		PLAYER_MSG->pSession = Client;

		{
			PLAYER_MSG->Request.m_args = std::tuple<DWORD, DWORD, string, string, string,INT64,string>
				(Client->GetId(), pBuffer->Reserve2,message.var_machine_id(), message.var_machine_name(), message.var_token(),message.var_index(), message.var_email());
		}

		PLAYER_MSG->Type = Client->GetMyDBTP();
		PLAYER_MSG->SubType = ONQUERY;
		MAINPROC.RegisterCommand(PLAYER_MSG);
	}