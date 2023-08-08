#pragma once

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace Hub {

	class GSAllocator;

	template<class T>
	class MSG_PLAYER_QUERY;


#define DECLARE_DB_CLASS_BEGIN(class_name) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
virtual ~class_name() {};\
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_1(class_name,type1) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_2(class_name,type1,type2) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
std::tuple<type1,type2> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\


#define DECLARE_DB_CLASS_BEGIN_3(class_name,type1,type2,type3) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_4(class_name,type1,type2,type3,type4) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_5(class_name,type1,type2,type3,type4,type5) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4,type5> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\


#define DECLARE_DB_CLASS_BEGIN_6(class_name,type1,type2,type3,type4,type5,type6) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4,type5,type6> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\


#define DECLARE_DB_CLASS_BEGIN_7(class_name,type1,type2,type3,type4,type5,type6,type7) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4,type5,type6,type7> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_END	\
	}\
		void Undo() {}\
}; 

	template<class T>
	class MSG_PLAYER_QUERY : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() = default;
		~MSG_PLAYER_QUERY() = default;

		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<T> pRequst;

		void Execute(LPVOID Param)
		{
			printf("error MSG_PLAYER_QUERY Type Not Define \n");
		}


		void Undo() {}
	};


#pragma region RequestDeleteAllConcurrentUser
	DECLARE_DB_CLASS_BEGIN(RequestDeleteAllConcurrentUser)
	pProcess->DeleteAllConcurrentUser();
	DECLARE_DB_CLASS_END
#pragma endregion


#pragma region CreateRooom
	DECLARE_DB_CLASS_BEGIN_4(CreateRooom, string, INT64, string, PlayerPtr)
	{
		CREATE_ROOM_RES res;

		PlayerPtr pPlayerPtr = std::get<3>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		auto ret = pProcess->CreateRoom(std::get<0>(Request.m_args), std::get<1>(Request.m_args), std::get<2>(Request.m_args));
		if (ret == -1)
		{
			res.set_var_code(SystemError);
			SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
				return;
		}


		ROOM_PTR RoomPtr = ROOMMGR.Create(ret, std::get<0>(Request.m_args));
		RoomPtr->m_Stock.Name = std::get<0>(Request.m_args);

		ROOMMGR.Add(RoomPtr, std::get<3>(Request.m_args));

		RoomPtr->InsertPlayer(std::get<3>(Request.m_args));

		res.set_var_room_id(RoomPtr->GetId());
		res.mutable_var_name()->assign(RoomPtr->m_Stock.Name);
		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())

			RoomPtr->SendNewUserInfo(std::get<3>(Request.m_args), RoomPtr->GetId());	//방에 있는 유저들에게 새로운 유저 정보전송f
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region EnterRooomDB
	DECLARE_DB_CLASS_BEGIN_4(EnterRooom, int, INT64, string, PlayerPtr)
	{

		PlayerPtr pPlayerPtr = std::get<3>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		ENTER_ROOM_RES res;


		ROOM_PTR RoomPtr = ROOMMGR.Search(std::get<0>(Request.m_args));
		if (RoomPtr == NULL)
		{
			res.set_var_code(SystemError);
			SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
				return;
		}

		auto ret = pProcess->EnterRoom(std::get<0>(Request.m_args), std::get<1>(Request.m_args), std::get<2>(Request.m_args));
		if (ret != 0)
		{
			res.set_var_code(SystemError);
			SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
				return;
		}

		RoomPtr->GetMessageList(res.mutable_var_messages());

		RoomPtr->InsertPlayer(pPlayerPtr);

		res.set_var_room_id(std::get<0>(Request.m_args));
		res.set_var_name(RoomPtr->m_Stock.Name.c_str());
		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())

			//새로 입장한 유저에게 방안의 유저 정보전송
			for each (auto iter in RoomPtr->m_PlayerMap)
			{
				if (iter.second == NULL)
					continue;

				NEW_USER_IN_ROOM_NTY nty;
				nty.set_var_type(0);
				RoomUserInfo* userinfo = nty.mutable_var_room_user();

				userinfo->set_var_index(iter.second->GetId());
				userinfo->set_var_name(iter.second->m_Account.GetName());
				userinfo->set_var_room_number(RoomPtr->GetId());

				GSCLIENT_PTR pPair = SERVER.GetClient(iter.second->GetFront());
				if (pPair == NULL)
					continue;

				SEND_PROTO_BUFFER(nty, pPair, iter.second->GetFrontSid())
			}


		//방안의 유저들 에게 새로운 유저 정보를 전송
		for each (auto iter in RoomPtr->m_PlayerMap)
		{
			if (iter.second == NULL)
				continue;

			GSCLIENT_PTR pPair = SERVER.GetClient(iter.second->GetFront());
			if (pPair == NULL)
				continue;

			NEW_USER_IN_ROOM_NTY nty;
			nty.set_var_type(1);
			RoomUserInfo* userinfo = nty.mutable_var_room_user();

			userinfo->set_var_index(pPlayerPtr->GetId());
			userinfo->set_var_room_number(RoomPtr->GetId());

			SEND_PROTO_BUFFER(nty, pPair, iter.second->GetFrontSid())
		}
	}
	DECLARE_DB_CLASS_END
#pragma endregion
#pragma region LeaveRoomDB
	DECLARE_DB_CLASS_BEGIN_4(LeaveRoom, int, INT64, string, PlayerPtr)
	{

		auto ret = pProcess->LeaveRoom(std::get<0>(Request.m_args), std::get<1>(Request.m_args), std::get<2>(Request.m_args));
		if (ret == -1)
		{
			return;
		}


		PlayerPtr pPlayerPtr = std::get<3>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		ROOMMGR.LeaveRoomPlayer(pPlayerPtr, std::get<0>(Request.m_args), ret == 1);
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RoomPassThrou
	DECLARE_DB_CLASS_BEGIN_4(RoomPassThrou, int, INT64, string, string)
	{
		std::time_t t = std::time(0);   // get time now
		std::tm now;

		localtime_s(&now, &t);

		string time;
		//("MM/dd/yyyy HH:mm:ss")
		time.append(to_string((now.tm_mon)));
		time.append("/");
		time.append(to_string((now.tm_mday)));
		time.append("/");
		time.append(to_string((now.tm_year + 1900)));
		time.append("/");
		time.append(" ");
		time.append(to_string((now.tm_hour)));
		time.append(":");
		time.append(to_string((now.tm_min)));
		time.append(":");
		time.append(to_string((now.tm_sec)));

		pProcess->AddRoomMessage(std::get<0>(Request.m_args), std::get<1>(Request.m_args), std::get<2>(Request.m_args), std::get<3>(Request.m_args), time);

	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region LoadDBRoomList
	DECLARE_DB_CLASS_BEGIN(LoadDBRoomList)
	{
		printf("LoadRoom Begin\n");
		auto ret = pProcess->LoadRooms(1000);
		for each (auto room  in ret)
		{
			auto room_id = std::get<0>(room);
			auto name = std::get<1>(room);
			auto pwd = std::get<2>(room);

			auto ROOMPTR = ROOMMGR.Create(room_id, name);
			ROOMMGR.Add2(ROOMPTR);

			/*
		(userid,name, room_id, time, msg)
			*/
			auto chatlist = pProcess->LoadRoomMessage(room_id, 100);
			for each (auto chat  in chatlist)
			{
				RoomMessage msg;

				msg.set_var_message(std::get<4>(chat));
				msg.set_var_name(std::get<1>(chat));
				msg.set_var_time(std::get<3>(chat));

				ROOMPTR->AddRoomMessage(msg);
			}

		}
		printf("LoadRoom End\n");


	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RegCamera
	DECLARE_DB_CLASS_BEGIN_3(RegCamera, string, string, PlayerPtr)
	{

		PlayerPtr pPlayerPtr = std::get<2>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		REG_CAMERA_RES res;

		auto ret = pProcess->RegisterCamera(pPlayerPtr->GetDBIndex(), std::get<0>(Request.m_args), std::get<1>(Request.m_args));

		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())

	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region DelCamera
	DECLARE_DB_CLASS_BEGIN_2(DelCamera, string,  PlayerPtr)
	{

		PlayerPtr pPlayerPtr = std::get<1>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		DEL_CAMERA_RES res;
		res.set_var_machine_id( std::get<0>(Request.m_args));

		auto ret = pProcess->DeleteCamera(pPlayerPtr->GetDBIndex(), std::get<0>(Request.m_args));

		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())

	}
	DECLARE_DB_CLASS_END
#pragma endregion


#pragma region RECONNECT
	DECLARE_DB_CLASS_BEGIN_7(Reconnect, DWORD,DWORD,string,string,string,INT64, string)
	{
		DWORD ForntId = std::get<0>(Request.m_args);
		DWORD FrontSid = std::get<1>(Request.m_args);

		std::string MachineId = std::get<2>(Request.m_args);
		string MachineName = std::get<3>(Request.m_args);

		string Token = std::get<4>(Request.m_args);


		INT64 index = std::get<5>(Request.m_args);
		string email = std::get<6>(Request.m_args);

		//해당 구글계정으로는 중복접속이 허용되지만 머신아이디가 동일한 것은 안된다 이전껏을 내보낸다.
		auto existClient = PLAYERMGR.SearchByMachineId(MachineId);
		if (existClient != NULL)
		{
			GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetFront());
			if (pPair != NULL)
			{
				BLOG("2.Login Fail Exist player %s and session close\n", MachineId);

				PLAYERMGR.Disconnect(existClient);

				CLIENT_KICK kick;
				SEND_PROTO_BUFFER(kick, pPair, FrontSid)
			}
		}


		PlayerPtr pNewPlayer = PLAYERMGR.Create();

		pNewPlayer->Initialize();

	

		pNewPlayer->SetMachineName(MachineName);
		pNewPlayer->SetMachineId(MachineId);

		pNewPlayer->SetFront(ForntId);
		pNewPlayer->SetFrontSid(FrontSid);

		pNewPlayer->SetDBIndex(index);


		pNewPlayer->SetPair(ForntId);
		//	pSession->SetPair(Index);

		pNewPlayer->m_AliveTime = GetTickCount();

		pNewPlayer->m_Email = email;
		pNewPlayer->m_Token = Token;
		pNewPlayer->m_Ip = "";

		PLAYERMGR.Add(pNewPlayer);

		RECONNECT_RES res;
		res.set_var_index(index);
		res.set_var_code(Success);
	

		//auto ret = pProcess->DeleteCamera(pPlayerPtr->GetDBIndex(), std::get<0>(Request.m_args));

		SEND_PROTO_BUFFER(res, pSession, FrontSid)

	}
	DECLARE_DB_CLASS_END
#pragma endregion


#pragma region LoadCamera List
	DECLARE_DB_CLASS_BEGIN_1(CameraList, PlayerPtr)
	{

		PlayerPtr pPlayerPtr = std::get<0>(Request.m_args);
		if (pPlayerPtr == NULL)
		{
			return;
		}

		CAMERA_LIST_RES res;
		res.set_var_code(Success);

		int count = 5;
		if (pPlayerPtr->m_PurchaseList.size() > 0)
			count = 999;

		auto camList = pProcess->RegCameraList(pPlayerPtr->GetDBIndex(), count);

		for each (auto & cam  in camList)
		{
			if (pPlayerPtr->GetMachineId() == std::get<1>(cam))
				continue;

			auto caminfo = res.add_var_camera();

			caminfo->set_var_machine_name(std::get<2>(cam).c_str());
			caminfo->set_var_machine_id(std::get<1>(cam).c_str());

			auto player = PLAYERMGR.SearchByMachineId(std::get<1>(cam).c_str());
			if (player != NULL)
			{
				caminfo->set_var_player_id(player->GetId());
				caminfo->set_var_ip(player->m_Ip.c_str());
			}
		}

		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())

	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestLogout
	DECLARE_DB_CLASS_BEGIN_1(RequestLogout, INT64)
	{
		pProcess->ProcedureUserLogout(std::get<0>(Request.m_args));
	}
	DECLARE_DB_CLASS_END
#pragma endregion


#pragma region RequestPrayList
	DECLARE_DB_CLASS_BEGIN_1(RequestPrayList, PlayerPtr)
	{
		PlayerPtr pPlayerPtr = std::get<0>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;

		auto praylist = pProcess->PrayList();

		PRAY_MESSAGE_RES res;
		res.set_var_code(Success);

		for each (auto pray in praylist)
		{
			auto info = res.add_var_list();
			info->mutable_var_name()->assign(std::get<0>(pray));
			info->mutable_var_message()->assign(std::get<1>(pray));
			info->mutable_var_time()->assign(std::get<2>(pray));
		}
		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestRegPray
	DECLARE_DB_CLASS_BEGIN_3(RequestRegPray, string, string, PlayerPtr)
	{
		PlayerPtr pPlayerPtr = std::get<2>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;

		pProcess->RegPray(std::get<1>(Request.m_args), std::get<0>(Request.m_args));

		PRAY_MESSAGE_RES res;
		res.set_var_code(Success);


		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestQNS
	DECLARE_DB_CLASS_BEGIN_3(RequestQNS, string, INT64, PlayerPtr)
	{
		

		boost::replace_all(std::get<0>(Request.m_args), "'", "''");

		int ret = pProcess->UpdaetQNS(std::get<1>(Request.m_args), std::get<0>(Request.m_args));

		ErrorCode code = ret == 0 ? ErrorCode::Success : ErrorCode::DataBaseError;

		QNA_RES res;
		res.set_var_code(code);

		PlayerPtr pPlayerPtr = std::get<2>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;
		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestNotice
	DECLARE_DB_CLASS_BEGIN_1(RequestNotice, PlayerPtr)
	{
		PlayerPtr pPlayerPtr = std::get<0>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;

		auto notice_list = pProcess->NoticeInfoInfo();

		NOTICE_RES res;

		for each (auto notice in notice_list)
		{
			auto data = res.add_var_list();

			data->set_var_id(std::get<0>(notice));
			data->set_var_content(std::get<1>(notice));
			data->set_var_date(std::get<2>(notice));
		}

		SEND_PROTO_BUFFER(res, pSession, pPlayerPtr->GetFrontSid())
	}
	DECLARE_DB_CLASS_END
#pragma endregion


#pragma region CalcRank
	DECLARE_DB_CLASS_BEGIN(CalcRank)
	{

	}
	DECLARE_DB_CLASS_END
#pragma endregion

	template<>
	class MSG_PLAYER_QUERY<RequestPlayerAuth> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestPlayerAuth pRequst;

		PlayerPtr pPlayer;

		std::vector<std::wstring> http_out;

		int GetGoogleAuth(string token)
		{
			try
			{
				// make uri
				http_client client(U("https://oauth2.googleapis.com/"));
				uri_builder builder(U("/tokeninfo"));
				builder.append_query(U("id_token"),
					token.c_str());

				auto requestTask = client.request(methods::GET, builder.to_string());
				requestTask.wait();

				http_response response = requestTask.get();
				if (response.status_code() == status_codes::OK)
				{
					auto V = response.extract_json().get();

					web::json::value _iss = V[U("iss")];
					if (_iss.is_string())
					{
						http_out.push_back(_iss.as_string());
					}

					web::json::value _sub = V[U("sub")];
					if (_sub.is_string())
					{
						http_out.push_back(_sub.as_string());
					}

					web::json::value _email = V[U("email")];
					if (_email.is_string())
					{
						http_out.push_back(_email.as_string());
					}

					web::json::value _name = V[U("name")];
					if (_name.is_string())
					{
						http_out.push_back(_T(_name.as_string()));
					}

					web::json::value _picture = V[U("picture")];
					if (_picture.is_string())
					{
						http_out.push_back(_picture.as_string());
					}
					web::json::value _locale = V[U("locale")];
					if (_locale.is_string())
					{
						http_out.push_back(_locale.as_string());
					}
				}

				return 0;

			}
			catch (const std::exception& e)
			{
				printf("Error exception:%s\n", e.what());
				return -1;
			}

			return 0;
		}

		void Execute(LPVOID Param)
		{
			try
			{

				LOGIN_RES res;

				if (pSession == NULL || pSession->GetConnected() == false)
				{
					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)
						return;
				}

				DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
				if (pProcess == NULL || pProcess->m_IsOpen == false)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search wong %d \n", pSession->GetMyDBTP());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)
					return;
				}

				auto result = GetGoogleAuth(pRequst.token);

				if (pRequst.token.size() == 0 || http_out.size() == 0)
				{
					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)


					PLAYERMGR.Disconnect(pSession);

					CLIENT_KICK kick;
					SEND_PROTO_BUFFER(kick, pSession, pRequst.FrontSid)
					return;
				}

				//해당 구글계정으로는 중복접속이 허용되지만 머신아이디가 동일한 것은 안된다 이전껏을 내보낸다.
				auto existClient = PLAYERMGR.SearchByMachineId(pRequst.MachineId);
				if (existClient != NULL)
				{
					GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetFront());
					if (pPair != NULL)
					{
						BLOG("2.Login Fail Exist player %s and session close\n", pRequst.MachineId.c_str());

						PLAYERMGR.Disconnect(existClient);

						CLIENT_KICK kick;
						SEND_PROTO_BUFFER(kick, pPair, pRequst.FrontSid)
					}
				}

				// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
				std::string authentickey;
				INT64 Index = 0;
				int score = 0, level = 0;

				std::string flatformid, google_name, picture_url, email, locale;

				flatformid.assign(http_out[1].begin(), http_out[1].end());
				email.assign(http_out[2].begin(), http_out[2].end());
				picture_url.assign(http_out[3].begin(), http_out[3].end());
				locale.assign(http_out[4].begin(), http_out[4].end());

				int nRet = pProcess->ProcedureUserLogin(flatformid, "", authentickey, score, Index, level);

				std::list<std::string> purchaselist = pProcess->GetPurchaseList(Index);

				PlayerPtr pNewPlayer = PLAYERMGR.Create();

				pNewPlayer->Initialize();

				//추후 수정
				pNewPlayer->SetName(google_name);


				pNewPlayer->SetMachineName(pRequst.MachineModel);
				pNewPlayer->SetMachineId(pRequst.MachineId);

				pNewPlayer->SetFront(pRequst.ForntId);
				pNewPlayer->SetFrontSid(pRequst.FrontSid);

				pNewPlayer->SetDBIndex(Index);


				pNewPlayer->SetPair(pRequst.ForntId);
				//	pSession->SetPair(Index);

				pNewPlayer->m_AliveTime = GetTickCount();

				pNewPlayer->m_Email = email;
				pNewPlayer->m_Token = pRequst.token;
				pNewPlayer->m_Ip = pRequst.Ip;
				pNewPlayer->m_PurchaseList = purchaselist;

				PLAYERMGR.Add(pNewPlayer);

	

				res.set_var_code(Success);
				res.set_var_index(Index);

				res.set_var_name(pRequst.token);
				res.set_var_email(email);
				res.set_var_machineid(pRequst.MachineId);
				res.set_var_machinename(pRequst.MachineModel);
				res.set_var_token(pRequst.token);

				SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)

				pNewPlayer->SetChannel(pRequst.channel);
			}
			catch (...)
			{
				printf("RequestPlayerAuth try catch \n");
			}
		}


		void Undo() {}
	};


	template<>
	class MSG_PLAYER_QUERY<RequestVerifyPurchase> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestVerifyPurchase pRequst;

		std::vector<std::wstring> http_out;

		int GetGoogleVerifyPurchase(string token, string packageid, string purchasename)
		{


			try
			{
				wstring _token;
				_token.assign(token.begin(), token.end());

				wstring _packageid;
				_packageid.assign(packageid.begin(), packageid.end());

				wstring _purchasename;
				_purchasename.assign(purchasename.begin(), purchasename.end());

				wstring out;

				out.append(U("http://localhost:8080/?token="));
				out.append(_token);
				out.append(U("&purchase_name="));
				out.append(_purchasename);
				out.append(U("&package_name="));
				out.append(_packageid);

				http_client client(out);

				auto requestTask = client.request(methods::GET);
				requestTask.wait();



				http_response response = requestTask.get();
				if (response.status_code() == status_codes::OK)
				{
					auto V = response.extract_json().get();

					web::json::value _iss = V[U("code")];
					if (_iss.is_string())
					{
						wstring ss = _iss.as_string();
						if (ss == U("ERR_BAD_REQUEST"))
							return 0;//실패
					}



					web::json::value _sub = V[U("orderId")];
					if (_sub.is_string())
					{
						wstring ss = _sub.as_string();

						string s1;
						s1.assign(ss.begin(), ss.end());
						printf("google verify purchase ssuccess %s", s1.c_str());

						return 1; //성공
					}

					//web::json::value _email = V[U("email")];
					//if (_email.is_string())
					//{
					//	http_out.push_back(_email.as_string());
					//}

					//web::json::value _name = V[U("name")];
					//if (_name.is_string())
					//{
					//	http_out.push_back(_T(_name.as_string()));
					//}

					//web::json::value _picture = V[U("picture")];
					//if (_picture.is_string())
					//{
					//	http_out.push_back(_picture.as_string());
					//}
					//web::json::value _locale = V[U("locale")];
					//if (_locale.is_string())
					//{
					//	http_out.push_back(_locale.as_string());
					//}
				}

				return 0;

			}
			catch (const std::exception& e)
			{
				printf("Error exception:%s\n", e.what());
				return -1;
			}

			return 0;
		}

		void Execute(LPVOID Param)
		{
			try
			{

				VERIFY_PURCHASE_RES res;

				if (pSession == NULL || pSession->GetConnected() == false)
				{
					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)
						return;
				}

				DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
				if (pProcess == NULL || pProcess->m_IsOpen == false)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search wong %d \n", pSession->GetMyDBTP());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)
						return;
				}

				int nRet = pProcess->InsertPurchase(pRequst.pPlayer->GetDBIndex(),pRequst.PurchaseId);


				auto result = GetGoogleVerifyPurchase(pRequst.token, pRequst.PacketId,pRequst.PurchaseId);

				if(result == 1)
					res.set_var_code(Success);
				else
					res.set_var_code(SystemError);

				SEND_PROTO_BUFFER(res, pSession, pRequst.FrontSid)

			}
			catch (...)
			{
				printf("RequestVerifyPurchase try catch \n");
			}
		}


		void Undo() {}
	};

	}



#include "MSG_PLAYER_QUERY.hpp"