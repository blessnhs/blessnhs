#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "../../DB/DBJob/DBContext.h"
#include "../../DB/DBProxy/DBProcessContainer.h"
#include "GSClient.h"
#include "../../PLAYER/Player.h"

#include "../../PLAYER/PlayerContainer.h"
#include "../../SERVER/GSHUB.h"

#include "GS.CLI.pb.h"
#include "Enum.pb.h"

#include <cpprest/containerstream.h>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/producerconsumerstream.h>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/tuple/tuple.hpp>
#include "Base64.h"
#include "../../Log/Log.h"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace Hub	{


	template<class T>
	class MSG_PLAYER_QUERY : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() = default;
		~MSG_PLAYER_QUERY() = default;

		DWORD			MsgId;

		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<T> pRequst;

		void Execute(LPVOID Param){}


		void Undo(){}
	};


	template<>
	class MSG_PLAYER_QUERY<RequestVersion> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			VERSION_RES res;

			if (pSession == NULL || pSession->GetConnected() == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			float nRet = pProcess->ProcedureVersion();

			res.set_var_version(nRet);
			res.set_var_code(Success);

			SEND_PROTO_BUFFER(res, pSession)
		}


		void Undo() {}
	};


	
	template<>
	class MSG_PLAYER_QUERY<RequestDeleteAllConcurrentUser> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			pProcess->DeleteAllConcurrentUser();
		}

		void Undo() {}
	};


	template<>
	class MSG_PLAYER_QUERY<CreateRooom> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		CreateRooom Request;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			CREATE_ROOM_RES res;

			auto ret = pProcess->CreateRoom(Request.room_name, Request.user_id, Request.user_name);
			if (ret == -1)
			{
				res.set_var_code(SystemError);
				SEND_PROTO_BUFFER(res, pSession)
				return;
			}


			ROOM_PTR RoomPtr = ROOMMGR.Create(ret, Request.room_name);
			RoomPtr->m_Stock.Name = Request.room_name;

			ROOMMGR.Add(RoomPtr, Request.pPlayer);

			RoomPtr->InsertPlayer(Request.pPlayer);

			res.set_var_room_id(RoomPtr->GetId());
			res.mutable_var_name()->assign(RoomPtr->m_Stock.Name);
			SEND_PROTO_BUFFER(res, pSession)

			RoomPtr->SendNewUserInfo(Request.pPlayer, RoomPtr->GetId());	//방에 있는 유저들에게 새로운 유저 정보전송f
		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<EnterRooomDB> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		EnterRooomDB Request;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			PlayerPtr pPlayer = PLAYERMGR.Search(pSession->GetPair());
			if (pPlayer == NULL)
			{
				return;
			}

			ENTER_ROOM_RES res;


			ROOM_PTR RoomPtr = ROOMMGR.Search(Request.room_id);
			if (RoomPtr == NULL)
			{
				res.set_var_code(SystemError);
				SEND_PROTO_BUFFER(res, pSession)
					return;
			}

			auto ret = pProcess->EnterRoom(Request.room_id, Request.user_id, Request.user_name);
			if (ret != 0)
			{
				res.set_var_code(SystemError);
				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			RoomPtr->GetMessageList(res.mutable_var_messages());

			RoomPtr->InsertPlayer(pPlayer);

			pPlayer->m_Char[0].SetRoom(Request.room_id);

			res.set_var_room_id(Request.room_id);
			res.set_var_name(RoomPtr->m_Stock.Name.c_str());
			SEND_PROTO_BUFFER(res, pSession)

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

				SEND_PROTO_BUFFER(nty, pSession)
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
				nty.set_var_type(1);
				RoomUserInfo* userinfo = nty.mutable_var_room_user();

				userinfo->set_var_index(pPlayer->GetId());
				userinfo->set_var_name(pPlayer->m_Char[0].GetName());
				userinfo->set_var_room_number(RoomPtr->GetId());

				SEND_PROTO_BUFFER(nty, pSession)
			}
		}

		void Undo() {}
	};


	template<>
	class MSG_PLAYER_QUERY<LeaveRoomDB> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;
		LeaveRoomDB Request;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}


			auto ret = pProcess->LeaveRoom(Request.room_id, Request.user_id, Request.user_name);
			if (ret ==  -1)
			{
				return;
			}


			PlayerPtr pPlayer = PLAYERMGR.Search(pSession->GetPair());
			if (pPlayer == NULL)
			{
				return;
			}

			ROOMMGR.LeaveRoomPlayer(pPlayer, Request.room_id, ret == 1);
		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<RoomPassThrou> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RoomPassThrou Request;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}


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

			pProcess->AddRoomMessage(Request.room_id, Request.user_id, Request.user_name, Request.msg, time);

		}

		void Undo() {}
	};
	
	template<>
	class MSG_PLAYER_QUERY<LoadDBRoomList> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestLogout pRequst;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

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
		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<RequestLogout> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestLogout pRequst;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			pProcess->ProcedureUserLogout(pRequst.Index);
		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<RequestPrayList> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

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
			SEND_PROTO_BUFFER(res, pSession)
		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<RequestRegPray> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestRegPray pRequst;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			pProcess->RegPray(pRequst.name, pRequst.contents);



			PRAY_MESSAGE_RES res;
			res.set_var_code(Success);

		

			SEND_PROTO_BUFFER(res, pSession)

		}

		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<RequestQNS> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		RequestQNS pRequst;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			boost::replace_all(pRequst.contents, "'", "''");

			int ret = pProcess->UpdaetQNS(pRequst.Index, pRequst.contents);

			ErrorCode code = ret == 0 ? ErrorCode::Success : ErrorCode::DataBaseError;

			QNA_RES res;
			res.set_var_code(code);

			SEND_PROTO_BUFFER(res, pSession)
		}

		void Undo() {}
	};


	template<>
	class MSG_PLAYER_QUERY<RequestNotice> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			if (pSession == NULL)
			{
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			auto notice_list  = pProcess->NoticeInfoInfo();

			NOTICE_RES res;

			for each (auto notice in notice_list)
			{
				auto data = res.add_var_list();

				data->set_var_id(std::get<0>(notice));
				data->set_var_content(std::get<1>(notice));
				data->set_var_date(std::get<2>(notice));
			}
		
			SEND_PROTO_BUFFER(res, pSession)
		}

		void Undo() {}
	};


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

					SEND_PROTO_BUFFER(res, pSession)
						return;
				}

				DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
				if (pProcess == NULL || pProcess->m_IsOpen == false)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search wong %d \n", pSession->GetMyDBTP());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession)
					return;
				}

				if (pRequst.id.size() == 0 || pRequst.id.size() > 256 || pRequst.pwd.size() == 0 || pRequst.pwd.size() > 256)
				{
					BLOG("DBPROCESSCONTAINER_CER.Search token size error %d \n", pRequst.id.size());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession)

					pSession->Close();
					return;
				}

			
				// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
				std::string authentickey;
				INT64 Index = 0;
				int score = 0,level = 0;


				//-1 비밀번호 다름
				//-2 이미 접속
				//-3 접속 로그남기기 실패
				int nRet = pProcess->ProcedureUserLogin(pRequst.id, pRequst.pwd, authentickey,score, Index, level);

				//이미 접속해 있는 세션이 있고(디비에 접속기록이 남아 있다.)
				if (nRet != _ERR_NONE)
				{
					//-1 비밀번호 다름
					//접속 종료만 처리
					if (nRet == -1)
					{
						BLOG("1.Login Fail Invalid Password %d  INDEX  %lld\n", nRet, Index);
						pSession->Close();
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
						SEND_PROTO_BUFFER(res, pSession)

						pSession->Close();

						//기존 세션과 신규 세션 양쪽 다 팅기는 것으로 변경
						//이미 접속중이면 이전 접속을 끊는다. 
						auto existClient = PLAYERMGR.Search(Index);
						if (existClient != NULL)
						{
							GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetPair());
							if (pPair != NULL)
							{
								BLOG("2.Login Fail Exist player %lld and session close\n", Index);

								pPair->Close();
							}
							return;
						}

						return;
					}
				}

				//해당 세센으로 이미 로그인을 했다.
				auto existClient = PLAYERMGR.Search(pSession->GetPair());
				if (existClient != NULL)
				{
					BLOG("Duplicate Login Fail Exist player %lld close\n", Index);
					pSession->Close();

					return;
				}

				PlayerPtr pNewPlayer = PLAYERMGR.Create();

				pNewPlayer->Initialize();
				pNewPlayer->m_Char[0].SetName(pRequst.id);
				pNewPlayer->m_Account.SetName(pRequst.id);

			
				pNewPlayer->SetId(Index);
				pNewPlayer->SetPair(pSession->GetId());
				pSession->SetPair(Index);
				pNewPlayer->m_Char[0].SetLevel(level);
				PLAYERMGR.Add(pNewPlayer);

				res.set_var_code(Success);
				res.set_var_index(Index);

				res.set_var_name(pRequst.id);

				SEND_PROTO_BUFFER(res, pSession)

				pNewPlayer->SetChannel(pRequst.channel);
			}
			catch (...)
			{
				printf("RequestPlayerAuth try catch \n");
			}
		}


		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY<INT> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() {  }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<INT> pRequst;

		void Execute(LPVOID Param){}


		void Undo(){}
	};
}

