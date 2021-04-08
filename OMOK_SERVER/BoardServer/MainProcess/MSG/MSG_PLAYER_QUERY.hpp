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

#include <boost/algorithm/string/replace.hpp>

#include "../../Channel/ChannelContainer.h"

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
	class MSG_PLAYER_QUERY<CalcRank> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			std::list<Rank> list;
			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			pProcess->CalcRank();

		}


		void Undo() {}
	};

	template<>
	class MSG_PLAYER_QUERY<NickNameCheck> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		NickNameCheck pRequst;
		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			CHECK_NICKNAME_RES res;
			
			bool ret = pProcess->NickNameCheck(pRequst.NickName, pRequst.Index);
			ErrorCode code = ret == true ? ErrorCode::Success : ErrorCode::DataBaseError;

			res.set_var_code(code);
			res.set_var_name(pRequst.NickName);

			SEND_PROTO_BUFFER(res, pSession)

		}


		void Undo() {}
	};


	template<>
	class MSG_PLAYER_QUERY<RequestRank> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		void Execute(LPVOID Param)
		{
			RANK_RES res;

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

			std::list<Rank> list;
			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			pProcess->RequestRank(list);

			for each (auto r in list)
			{
				if (r.var_win() == 0)
					continue;

				auto rank = res.mutable_var_rank_list()->Add();

				*rank = r;
			}


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

			QNS_RES res;
			res.set_var_code(code);

			SEND_PROTO_BUFFER(res, pSession)
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

				if (pRequst.Token.size() == 0 || pRequst.Token.size() < 10)
				{
					printf("DBPROCESSCONTAINER_CER.Search token size error %d \n", pRequst.Token.size());

					res.set_var_code(DataBaseError);

					SEND_PROTO_BUFFER(res, pSession)

					pSession->Close();
					return;
				}

				for (int i = 0; i < 1; i++)
				{
					GetGoogleAuth(pRequst.Token.c_str());

					if (http_out.size() == 6)
						break;
				}
	
				if (http_out.size() != 6)
				{
					pSession->Close();
					return;
				}

				std::string flatformid, google_name, picture_url, email, locale;

				flatformid.assign(http_out[1].begin(), http_out[1].end());
				email.assign(http_out[2].begin(), http_out[2].end());
				picture_url.assign(http_out[4].begin(), http_out[4].end());
				locale.assign(http_out[5].begin(), http_out[5].end());

				Base64::convert_unicode_to_ansi_string(google_name, http_out[3].c_str(), http_out[3].size());

				int rank, score, win, lose, draw, level;
				string nickname;

				// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
				std::string authentickey;
				INT64 Index = 0;
				WORD nRet = pProcess->ProcedureUserLogin(flatformid.c_str(), 0/*0번은 구글 인증*/, picture_url.c_str(), email.c_str(), locale.c_str(), authentickey, rank, score, win, lose, draw, Index, level, nickname);

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
					res.set_var_code(LoginFailed);
					SEND_PROTO_BUFFER(res, pSession)
						return;
				}

				PlayerPtr pNewPlayer = PLAYERMGR.Create();

				pNewPlayer->Initialize();
				pNewPlayer->m_Char[0].SetName(nickname);
				pNewPlayer->m_Account.SetFlatformId(flatformid);
				pNewPlayer->m_Account.SetEMail(email);
				pNewPlayer->m_Account.SetPicture_url(picture_url);
				pNewPlayer->m_Account.SetName(nickname);

				pNewPlayer->SetId(Index);
				pNewPlayer->SetPair(pSession->GetId());
				pSession->SetPair(Index);
				pNewPlayer->m_Char[0].GetScore().SetScore(rank, score, win, lose, draw);
				pNewPlayer->m_Char[0].SetLevel(level);
				PLAYERMGR.Add(pNewPlayer);

				res.set_var_code(Success);
				res.set_var_index(Index);

				res.set_var_win(win);
				res.set_var_lose(lose);
				res.set_var_draw(draw);
				res.set_var_score(score);
				res.set_var_rank(rank);
				res.set_var_level(level);
				res.set_var_locale(locale);
				res.set_var_name(nickname);

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

	template<>
	class MSG_PLAYER_QUERY<RequestPlayerScore> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() {  }
		~MSG_PLAYER_QUERY() {}

		DBPROCESS_CER_PTR   pProcess;

		RequestPlayerScore pRequst;

		void Execute(LPVOID Param) 
		{
			//유저가종료되어도 승패는 기록되어야 하기에 디폴트 디비 핸들러를 사용한다.
			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				return;
			}

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			int nRet = pProcess->UpdaetPlayerScore(pRequst.Index, pRequst.Win, pRequst.Lose, pRequst.Draw, pRequst.Level, pRequst.Score);
			if (nRet != 0)
			{
				printf("UpdaetPlayerScore nRet is not 0\n");
			}
		}


		void Undo() {}
	};
	
}

