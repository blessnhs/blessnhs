#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "../../DB/DBJob/DBContext.h"
#include "../../DB/DBProxy/DBProcessContainer.h"
#include "GSClient.h"

#include "../../PLAYER/Container/PlayerContainer.h"
#include "../../SERVER/GSBoard.h"

#include "GS.CLI.pb.h"
#include "Enum.pb.h"

#include <cpprest/containerstream.h>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/producerconsumerstream.h>
#include <iostream>
#include <sstream>

#include "Base64.h"

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace Board	{


	template<class T>
	class MSG_PLAYER_QUERY : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

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

		boost::shared_ptr<RequestVersion> pRequst;

		void Execute(LPVOID Param)
		{
			VERSION_RES res;

			if (pSession == NULL || pSession->GetConnected() == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
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

		boost::shared_ptr<RequestDeleteAllConcurrentUser> pRequst;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(MSG_TYPE_DB_1);
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

		boost::shared_ptr<RequestLogout> pRequst;

		void Execute(LPVOID Param)
		{
			if (pSession == NULL)
			{
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			pProcess->ProcedureUserLogout(pRequst->Index);
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

		boost::shared_ptr<RequestPlayerAuth> pRequst;

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


				// make task(request and get result)
				auto requestTask = client.request(web::http::methods::GET, builder.to_string())
				// The following code executes when the response is available
				.then([](http_response response) -> pplx::task<json::value>
				{
					std::wostringstream stream;
					stream.str(std::wstring());
					stream << L"Content type: " << response.headers().content_type() << std::endl;
					stream << L"Content length: " << response.headers().content_length() << L"bytes" << std::endl;
					std::wcout << stream.str();

					// If the status is OK extract the body of the response into a JSON value
					if (response.status_code() == status_codes::OK)
					{
						return response.extract_json();
					}
					else
					{
						// return an empty JSON value
						return pplx::task_from_result(json::value());
					}
					// Continue when the JSON value is available
				}).then([](pplx::task<json::value> previousTask)
				{
					// Get the JSON value from the task and call the DisplayJSONValue method
					try
					{
						std::vector<std::wstring> outValue;

						json::value const& V = previousTask.get();
						{
							web::json::value _iss = V.at(U("iss"));
							if (_iss.is_string())
							{
								outValue.push_back(_iss.as_string());
							}

							web::json::value _sub = V.at(U("sub"));
							if (_sub.is_string())
							{
								outValue.push_back(_sub.as_string());
							}

							web::json::value _email = V.at(U("email"));
							if (_email.is_string())
							{
								outValue.push_back(_email.as_string());
							}

							web::json::value _name = V.at(U("name"));
							if (_name.is_string())
							{
								outValue.push_back(_T(_name.as_string()));
							}

							web::json::value _picture = V.at(U("picture"));
							if (_picture.is_string())
							{
								outValue.push_back(_picture.as_string());
							}

							return outValue;
						}
					}
					catch (http_exception const& e)
					{
						std::wcout << e.what() << std::endl;
					}

					return std::vector<std::wstring>();
				});
				
				requestTask.wait();

				http_out = requestTask.get();

			}
			catch (const std::exception & e)
			{
				printf("Error exception:%s\n", e.what());
			}

			return 0;
		}

		void Execute(LPVOID Param)
		{
			LOGIN_RES res;

			if (pSession == NULL || pSession->GetConnected() == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			GetGoogleAuth(pRequst->Token.c_str());

			if(http_out.size() != 5)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			std::string flatformid, name, picture_url, email;

			flatformid.assign(http_out[1].begin(), http_out[1].end());
			email.assign(http_out[2].begin(), http_out[2].end());
			picture_url.assign(http_out[4].begin(), http_out[4].end());

			Base64::convert_unicode_to_ansi_string(name, http_out[3].c_str(), http_out[3].size());

			int rank, score, win, lose, draw, level, level_point;

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			std::string authentickey;
			INT64 Index = 0;
			WORD nRet = pProcess->ProcedureUserLogin(flatformid.c_str(), 0/*0번은 구글 인증*/, name.c_str(),picture_url.c_str(),email.c_str(), authentickey, rank, score, win, lose, draw,Index, level, level_point);

			if(nRet != _ERR_NONE )
			{ 
				res.set_var_code(LoginFailed);
				SEND_PROTO_BUFFER(res, pSession)
				return ;
			}

			//이미 접속중이면 이전 접속을 끊는다. 
			auto existClient = PLAYERMGR.Search(Index);
			if (existClient != NULL)
			{
				GSCLIENT_PTR pPair = SERVER.GetClient(existClient->GetPair());
				if (pPair != NULL)
				{
					pPair->Close();
				}

				res.set_var_code(LoginFailed);
				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			PlayerPtr pNewPlayer = PLAYERMGR.Create();
	
			pNewPlayer->Initialize();
			pNewPlayer->m_Char[0].SetName(name);
			pNewPlayer->m_Account.SetFlatformId(flatformid);
			pNewPlayer->m_Account.SetEMail(email);
			pNewPlayer->m_Account.SetPicture_url(picture_url);
			pNewPlayer->m_Account.SetName(name);

			pNewPlayer->SetId(Index);
			pNewPlayer->SetPair(pSession->GetId());
			pSession->SetPair(Index);
			pNewPlayer->m_Char[0].GetScore().SetScore(rank, score, win, lose, draw);
			pNewPlayer->m_Char[0].SetLevel(level);
			pNewPlayer->m_Char[0].SetLevelPoint(level_point);
			PLAYERMGR.Add(pNewPlayer);

			res.set_var_code(Success);

			res.set_var_index(Index);
			SEND_PROTO_BUFFER(res, pSession)
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

	template<>
	class MSG_PLAYER_QUERY<RequestPlayerScore> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() {  }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR		pSession;

		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<RequestPlayerScore> pRequst;

		void Execute(LPVOID Param) 
		{
			if (pSession == NULL || pSession->GetConnected() == false)
			{
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			if (pRequst == NULL)
				return;

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			int nRet = pProcess->UpdaetPlayerScore(pRequst->Index, pRequst->Win, pRequst->Lose, pRequst->Draw);
			if (nRet != 0)
			{
				printf("UpdaetPlayerScore nRet is not 0\n");
			}
		}


		void Undo() {}
	};
	
}

