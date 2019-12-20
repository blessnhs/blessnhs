
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
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		GSCLIENT			*pSession;
		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<RequestPlayerAuth> pRequst;

		void Execute(LPVOID Param)
		{
			if(pSession->GetConnected() == false)
				return ;

			pProcess = DBPROCESSCONTAINER_CER.Search(pSession->GetMyDBTP());
			if(pProcess == NULL)
				return ;

			boost::shared_ptr<RequestPlayerAuth> res(pRequst);

			// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
			WCHAR authentickey[64] = {0,};
			WORD nRet = pProcess->GetAuthenticKeyFromDB(res->Account.c_str(), res->Passwd.c_str(), authentickey);


			PlayerPtr pNewPlayer = PLAYERMGR.Create();
			pNewPlayer->Initialize();
/*			pNewPlayer->m_Char[0].SetName(CharName);
			pNewPlayer->m_Account.SetName(Account);
			pNewPlayer->SetId(m_Index);
			pNewPlayer->m_UnityServerId = pOwner->GetId();

			pNewPlayer->SetPair(SessionId);
			*/


			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("MPID",ID_AUTH_LOGIN_RES)

			PKT_AUTHENCATION_RES SendData;
			if(nRet == _ERR_LOGINED)
			{
				ADD_JSON_MEMBER("Result",_ERR_LOGINED)
				END_JSON_MEMBER(pSession->GetTCPSocket(),(WORD)ID_AUTH_LOGIN_RES)

				return ;
			}

			if(nRet != _ERR_NONE)
			{ 
				ADD_JSON_MEMBER("Result",_ERR_LOGINED)
				END_JSON_MEMBER(pSession->GetTCPSocket(),(WORD)ID_AUTH_LOGIN_RES)
				return ;
			}

			std::wstring FrontIp = INI.Default_FrontIp;
			std::wstring Key     = authentickey;

			ADD_JSON_WSTR_MEMBER("IpAddress",FrontIp)
			ADD_JSON_MEMBER("Port",INI.Default_FrontPort)
			ADD_JSON_WSTR_MEMBER("SessionKey",Key)
			ADD_JSON_WSTR_MEMBER("Id",res->Account)
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

