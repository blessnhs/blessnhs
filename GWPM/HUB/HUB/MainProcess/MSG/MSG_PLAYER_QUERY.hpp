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

			// �α��� ���� : ���̵��� ����Ȯ�� �� ����Ű���� �����´�.
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

			
				// �α��� ���� : ���̵��� ����Ȯ�� �� ����Ű���� �����´�.
				std::string authentickey;
				INT64 Index = 0;
				int score = 0,level = 0;


				//-1 ��й�ȣ �ٸ�
				//-2 �̹� ����
				//-3 ���� �α׳���� ����
				int nRet = pProcess->ProcedureUserLogin(pRequst.id, pRequst.pwd, authentickey,score, Index, level);

				//�̹� ������ �ִ� ������ �ְ�(��� ���ӱ���� ���� �ִ�.)
				if (nRet != _ERR_NONE)
				{
					//-1 ��й�ȣ �ٸ�
					//���� ���Ḹ ó��
					if (nRet == -1)
					{
						BLOG("1.Login Fail Invalid Password %d  INDEX  %lld\n", nRet, Index);
						pSession->Close();
						return;
					}
					//- 2 �̹� ����
					// ���� �� ���� ó��
					else if (nRet == -2)
					{
						//��� ���� ����� �׳� ������. ������ ���� �� �ñ��. 
						pProcess->ProcedureUserLogout(Index);

						BLOG("2.Login Fail Concurrent Table Exist data Ret %d  INDEX  %lld\n", nRet, Index);
						res.set_var_code(LoginFailed);
						SEND_PROTO_BUFFER(res, pSession)

						pSession->Close();

						//���� ���ǰ� �ű� ���� ���� �� �ñ�� ������ ����
						//�̹� �������̸� ���� ������ ���´�. 
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

				//�ش� �������� �̹� �α����� �ߴ�.
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

