#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


#include "../../DB/DBJob/DBContext.h"
#include "../../DB/DBProxy/DBProcessContainer.h"
#include "GSClient.h"

#include "../../PLAYER/Container/PlayerContainer.h"
#include "../../SERVER/GSBoard.h"

#include "GS.CLI.pb.h"
#include "Enum.pb.h"

using namespace google;

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
	class MSG_PLAYER_QUERY<RequestPlayerAuth> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { }
		~MSG_PLAYER_QUERY() {}

		GSCLIENT_PTR pSession;

		boost::shared_ptr<RequestPlayerAuth> pRequst;

		void Execute(LPVOID Param)
		{
			LOGIN_RES res;

			if (pSession == NULL || pSession->GetConnected() == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP(MSG_TYPE_DB_1));
			if (pProcess == NULL || pProcess->m_IsOpen == false)
			{
				res.set_var_code(DataBaseError);

				SEND_PROTO_BUFFER(res, pSession)
				return;
			}

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			std::wstring authentickey;
			INT64 Index = 0;
			WORD nRet = pProcess->ProcedureUserLogin(pRequst->Account.c_str(), pRequst->Passwd.c_str(), authentickey, Index);

			if(nRet != _ERR_NONE || (PLAYERMGR.Search(pRequst->Account) != NULL))
			{ 
				res.set_var_code(LoginDuplicate);
				return ;
			}

			PlayerPtr pNewPlayer = PLAYERMGR.Create();
	
			pNewPlayer->Initialize();
			pNewPlayer->m_Char[0].SetName(pRequst->Account);
			pNewPlayer->m_Account.SetName(pRequst->Account);
			pNewPlayer->SetId(Index);
			pNewPlayer->SetPair(pSession->GetId());
			pSession->SetPair(Index);
			PLAYERMGR.Add(pNewPlayer);


			res.set_var_code(Success);

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
	
}

