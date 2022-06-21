#pragma once

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace Hub	{

	class GSAllocator;

	template<class T>
	class MSG_PLAYER_QUERY ;


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
	DECLARE_DB_CLASS_BEGIN_4(CreateRooom,string,INT64,string,PlayerPtr)
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
			SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(),res, pSession)
			return;
		}


		ROOM_PTR RoomPtr = ROOMMGR.Create(ret, std::get<0>(Request.m_args));
		RoomPtr->m_Stock.Name = std::get<0>(Request.m_args);

		ROOMMGR.Add(RoomPtr, std::get<3>(Request.m_args));

		RoomPtr->InsertPlayer(std::get<3>(Request.m_args));

		res.set_var_room_id(RoomPtr->GetId());
		res.mutable_var_name()->assign(RoomPtr->m_Stock.Name);
		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(),res, pSession)

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
			SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
			return;
		}

		auto ret = pProcess->EnterRoom(std::get<0>(Request.m_args), std::get<1>(Request.m_args), std::get<2>(Request.m_args));
		if (ret != 0)
		{
			res.set_var_code(SystemError);
			SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
			return;
		}

		RoomPtr->GetMessageList(res.mutable_var_messages());

		RoomPtr->InsertPlayer(pPlayerPtr);

		pPlayerPtr->m_Char[0].SetRoom(std::get<0>(Request.m_args));

		res.set_var_room_id(std::get<0>(Request.m_args));
		res.set_var_name(RoomPtr->m_Stock.Name.c_str());
		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)

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

				SEND_PROTO_BUFFER2(iter.second->GetFrontSid(), nty, pPair)
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
			userinfo->set_var_name(pPlayerPtr->m_Char[0].GetName());
			userinfo->set_var_room_number(RoomPtr->GetId());

			SEND_PROTO_BUFFER2(iter.second->GetFrontSid(),nty, pPair)
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
	DECLARE_DB_CLASS_BEGIN_4(RoomPassThrou, int, INT64, string,string)
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

#pragma region RequestLogout
	DECLARE_DB_CLASS_BEGIN_1(RequestLogout,INT64)
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
		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestRegPray
	DECLARE_DB_CLASS_BEGIN_3(RequestRegPray,string,string, PlayerPtr)
	{
		PlayerPtr pPlayerPtr = std::get<2>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;

		pProcess->RegPray(std::get<1>(Request.m_args), std::get<0>(Request.m_args));

		PRAY_MESSAGE_RES res;
		res.set_var_code(Success);


		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
	}
	DECLARE_DB_CLASS_END
#pragma endregion

#pragma region RequestQNS
	DECLARE_DB_CLASS_BEGIN_3(RequestQNS, string, INT64, PlayerPtr)
	{
		PlayerPtr pPlayerPtr = std::get<2>(Request.m_args);
		if (pPlayerPtr == NULL)
			return;

		boost::replace_all(std::get<0>(Request.m_args), "'", "''");

		int ret = pProcess->UpdaetQNS(std::get<1>(Request.m_args), std::get<0>(Request.m_args));

		ErrorCode code = ret == 0 ? ErrorCode::Success : ErrorCode::DataBaseError;

		QNA_RES res;
		res.set_var_code(code);

		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
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

		SEND_PROTO_BUFFER2(pPlayerPtr->GetFrontSid(), res, pSession)
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
			catch (const std::exception & e)
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

					SEND_PROTO_BUFFER2(pRequst.FrontSid,res, pSession)
					return;
				}

				DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
				if (pProcess == NULL || pProcess->m_IsOpen == false)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search wong %d \n", pSession->GetMyDBTP());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER2(pRequst.FrontSid, res, pSession)
					return;
				}

				if (pRequst.id.size() == 0 || pRequst.id.size() > 256 || pRequst.pwd.size() == 0 || pRequst.pwd.size() > 256)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search token size error %d \n", pRequst.id.size());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER2(pRequst.FrontSid, res, pSession)

					//pSession->Close();

					PLAYERMGR.Disconnect(pSession);

					CLIENT_KICK kick;
					SEND_PROTO_BUFFER2(pRequst.FrontSid, kick, pSession)
					return;
				}


				// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
				std::string authentickey;
				INT64 Index = 0;
				int score = 0, level = 0;


				//-1 비밀번호 다름
				//-2 이미 접속
				//-3 접속 로그남기기 실패
				int nRet = pProcess->ProcedureUserLogin(pRequst.id, pRequst.pwd, authentickey, score, Index, level);

				//이미 접속해 있는 세션이 있고(디비에 접속기록이 남아 있다.)
				if (nRet != _ERR_NONE)
				{
					//-1 비밀번호 다름
					//접속 종료만 처리
					if (nRet == -1)
					{
						BLOG("1.Login Fail Invalid Password %d  INDEX  %lld\n", nRet, Index);
						//pSession->Close();

						PLAYERMGR.Disconnect(pSession);

						CLIENT_KICK kick;
						SEND_PROTO_BUFFER2(pRequst.FrontSid, kick, pSession)
						return;
					}
					//- 2 이미 접속
					// 양쪽 다 종료 처리
					else if (nRet == -2)
					{
						//디비에 접속 기록을 그냥 날린다. 어차피 양쪽 다 팅긴다. 
						pProcess->ProcedureUserLogout(Index);

						BLOG("2.Login Fail Concurrent Table Exist data Ret %d  INDEX  %lld\n", nRet, Index);
						res.set_var_code(LoginFailed);
						SEND_PROTO_BUFFER2(pRequst.FrontSid, res, pSession)


						CLIENT_KICK kick;
						SEND_PROTO_BUFFER2(pRequst.FrontSid, kick, pSession)
						//pSession->Close();

						PLAYERMGR.Disconnect(pSession);

						//기존 세션과 신규 세션 양쪽 다 팅기는 것으로 변경
						//이미 접속중이면 이전 접속을 끊는다.
						//다른쓰레드에서 아직 캐릭터를 생성하기 전이면 못찾을수도 있다.
						auto existClient = PLAYERMGR.Search(Index);
						if (existClient != NULL)
						{
							GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetFront());
							if (pPair != NULL)
							{
								BLOG("2.Login Fail Exist player %lld and session close\n", Index);

								PLAYERMGR.Disconnect(existClient);

								CLIENT_KICK kick;
								SEND_PROTO_BUFFER2(existClient->GetFrontSid(), kick, pSession)
							}
							return;
						}

						return;
					}
				}

				//해당 세센으로 이미 로그인을 했다.
				auto existClient = PLAYERMGR.Search(pRequst.ForntId,pRequst.FrontSid);
				if (existClient != NULL)
				{
					BLOG("Duplicate Login Fail Exist player %lld close\n", Index);
					//pSession->Close();


					PLAYERMGR.Disconnect(existClient);

					CLIENT_KICK kick;
					SEND_PROTO_BUFFER2(pRequst.FrontSid, kick, pSession)

					return;
				}

				//해당 디비 index 유저가이미 존재
				existClient = PLAYERMGR.Search(Index);
				if (existClient != NULL)
				{
					GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetFront());
					if (pPair != NULL)
					{
						BLOG("3.Login Fail Exist player %lld and session close\n", Index);


						CLIENT_KICK kick1;
						SEND_PROTO_BUFFER2(existClient->GetFrontSid(), kick1, pSession)

						PLAYERMGR.Disconnect(pPair);

						PLAYERMGR.Disconnect(pSession);

					//	pPair->Close();

					// pSession->Close();

						CLIENT_KICK kick2;
						SEND_PROTO_BUFFER2(pRequst.FrontSid, kick2, pSession)
					}
					return;
				}

				PlayerPtr pNewPlayer = PLAYERMGR.Create();

				pNewPlayer->Initialize();
				pNewPlayer->m_Char[0].SetName(pRequst.id);
				pNewPlayer->m_Account.SetName(pRequst.id);


				pNewPlayer->SetFront(pRequst.ForntId);
				pNewPlayer->SetFrontSid(pRequst.FrontSid);


				pNewPlayer->SetId(Index);
				
				
				pNewPlayer->SetPair(pRequst.ForntId);
			//	pSession->SetPair(Index);

				pNewPlayer->m_Char[0].SetLevel(level);

				pNewPlayer->m_AliveTime = GetTickCount();

				PLAYERMGR.Add(pNewPlayer);

				res.set_var_code(Success);
				res.set_var_index(Index);

				res.set_var_name(pRequst.id);

				SEND_PROTO_BUFFER2(pRequst.FrontSid, res, pSession)

				pNewPlayer->SetChannel(pRequst.channel);
			}
			catch (...)
			{
				printf("RequestPlayerAuth try catch \n");
			}
		}


		void Undo() {}
	};

}



#include "MSG_PLAYER_QUERY.hpp"