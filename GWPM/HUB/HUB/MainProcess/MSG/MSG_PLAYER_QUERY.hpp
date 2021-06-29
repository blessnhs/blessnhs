#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "../../DB/DBJob/DBContext.h"
#include "../../DB/DBProxy/DBProcessContainer.h"
#include "GSClient.h"

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

			string notice;

			pProcess->NoticeInfoInfo(notice);

			NOTICE_RES res;
			res.set_var_message(notice.c_str());

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
					printf("DBPROCESSCONTAINER_CER.Search wong %d \n", pSession->GetMyDBTP());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession)
						return;
				}

				if (pRequst.id.size() == 0 || pRequst.id.size() > 256 || pRequst.pwd.size() == 0 || pRequst.pwd.size() > 256)
				{
					printf("DBPROCESSCONTAINER_CER.Search token size error %d \n", pRequst.id.size());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession)

					pSession->Close();
					return;
				}

			
				// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
				std::string authentickey;
				INT64 Index = 0;
				int score = 0,level = 0;
				WORD nRet = pProcess->ProcedureUserLogin(pRequst.id, pRequst.pwd, authentickey,score, Index, level);

				//이미 접속해 있는 세션이 있고(디비에 접속기록이 남아 있다.)
				if (nRet != _ERR_NONE)
				{
				//	printf("Login Fail Concurrent Table Exist data Ret %d \n", nRet);
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
					//		printf("Exist player client %lld and session close\n", Index);

							pPair->Close();
						}
						return;
					}

					return;
				}

				//이미 접속중이면 이전 접속을 끊는다. 
				auto existClient = PLAYERMGR.Search(Index);
				if (existClient != NULL)
				{
					GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetPair());
					if (pPair != NULL)
					{
				//		printf("1.Exist player client %lld \n", Index);

						pPair->Close();
					}

			//		printf("2.Exist player client %d \n", Index);
			//		res.set_var_code(LoginFailed);
			//		SEND_PROTO_BUFFER(res, pSession)
			//			return;
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

