
#include "../../DB/DBProxy/DBProcess.h"
#include "../../DB/DBProxy/DBProcessContainer.h"

#include "../MSG/MSG_PLAYER_QUERY_RES.h"

#include "Protocol.h"

#include "Auth.h"
#include "../../DB/DBJob/DBContext.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSSerialize.h"
#include "GSClient.h"

#include "../../Ini/INI.h"

#include "../../PLAYER/Container/PlayerContainer.h"
#include "../../SERVER/GSBoard.h"

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
		~MSG_PLAYER_QUERY(){}

		GSCLIENT_PTR pSession;

		boost::shared_ptr<RequestPlayerAuth> pRequst;

		void Execute(LPVOID Param)
		{

			if(pSession == NULL || pSession->GetConnected() == false)
				return ;

			DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
			if(pProcess == NULL)
				return ;

			if (PLAYERMGR.Search(pRequst->Account.c_str()) != NULL)
			{
				return;
			}

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			std::wstring authentickey;
			INT64 Index = 0;
			WORD nRet = pProcess->ProcedureUserLogin(pRequst->Account.c_str(), pRequst->Passwd.c_str(), authentickey, Index);

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("MPID",ID_AUTH_LOGIN_RES)

			if(nRet != _ERR_NONE)
			{ 
				ADD_JSON_MEMBER("Result",_ERR_LOGINED)
				END_JSON_MEMBER(pSession->GetTCPSocket(),(WORD)ID_AUTH_LOGIN_RES)
				return ;
			}

			PlayerPtr pNewPlayer = PLAYERMGR.Create();
			pNewPlayer->Initialize();
			pNewPlayer->m_Char[0].SetName(pRequst->Account.c_str());
			pNewPlayer->m_Account.SetName(pRequst->Account.c_str());
			pNewPlayer->SetId(Index);
			pNewPlayer->SetPair(pSession->GetId());
			PLAYERMGR.Add(pNewPlayer);


			ADD_JSON_WSTR_MEMBER("SessionKey", authentickey)
			ADD_JSON_WSTR_MEMBER("Id", pRequst->Account)
			ADD_JSON_MEMBER("Result",_ERR_NONE)

			END_JSON_MEMBER(pSession->GetTCPSocket(),(WORD)ID_AUTH_LOGIN_RES)
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

