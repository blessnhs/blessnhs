
#include "MSG_PLAYER_QUERY.h"


#include "../../DBProxy/DBProcess.h"
#include "../../DBProxy/DBJob/RequestPlayerAuth.h"
#include "../../DBProxy/DBJob/UpdatePlayerData.h"
#include "../../DBProxy/DBJob/ClosePlayer.h"
#include "../../DBProxy/DBJob/ClearConcurrent.h"
#include "../../DBProxy/DBProcessContainer.h"

#include "../../PLAYER/Message/MSG_PLAYER_QUERY_RES.h"

#include "../GSPacket/Protocol.h"
#include "../../PLAYER/Container/Player.h"
#include <boost/shared_ptr.hpp>
#include "../../LOG/FrontLog.h"

#include "GSAllocator.h"

#include "GSServer.h"

namespace GSFrontServer	{

	template<class T>
	class MSG_PLAYER_QUERY : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		GSCLIENT		*pSession;
		DBPROCESS_PTR   pProcess;

		T pRequst;

		void Execute(LPVOID Param)
		{
		}
		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY<RequestPlayerAuth> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD				MsgId;

		GSCLIENT			*pSession;
		DBPROCESS_PTR		pProcess;

		boost::shared_ptr<RequestPlayerAuth> pRequst;

		void Execute(LPVOID Param)
		{
			pProcess = DBPROCESSCONTAINER.Search(pSession->GetMyDBTP());
			if(pProcess == NULL)
			{
				printf("Not Found DB Handle");
				return ;
			}

			boost::shared_ptr<RequestPlayerAuth> res = pRequst;
			if(pSession != NULL)
			{
				if(pSession->GetConnected() == false)
				{
					return ;
				}

				res->Index = 0;

				res->pClient = pSession;

				res->SetId(DBP_GET_LOGIN_INFO_RE);
				res->Index = pProcess->ProcedureUserLogin(res->Account.c_str(),res->SessionKey.c_str(),0);

				if(res->Index != 0)
					res->Result = pProcess->SelectCharacterInfo(res->Account.c_str(),*res);
				else
					printf("2.MSG_PLAYER_QUERY _PROTOCOL_DBA::DBP_GET_LOGIN_INFO index is  %S %S\n",res->Account.c_str(),res->SessionKey.c_str());

				PROC_REG_QUERY_RES_JOB(res,pSession,res->Result,MSG_PLAYER_QUERY_RES,RequestPlayerAuth)
				
			}
	
		}

		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY<ClosePlayer> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		GSCLIENT			*pSession;
		DBPROCESS_PTR   pProcess;

		boost::shared_ptr<ClosePlayer> pRequst;

		void Execute(LPVOID Param)
		{
			if(pSession != NULL)
			{
				pProcess = DBPROCESSCONTAINER.Search(pSession->GetMyDBTP());
				if(pProcess == NULL)
				{
					printf("Not Found DB Handle");
					return ;
				}

				pProcess->ClosePlayer((WCHAR *)pRequst->AccountId.c_str());
			}
			else
			{
				printf("MSG_PLAYER_QUERY is Null \n");
			}
		}
		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY<ClearConcurrent> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		GSCLIENT			*pSession;
		DBPROCESS_PTR   pProcess;

		boost::shared_ptr<ClearConcurrent> pRequst;

		void Execute(LPVOID Param)
		{
			pProcess = DBPROCESSCONTAINER.Search(1);
			if(pProcess == NULL)
			{
				printf("Not Found DB Handle");
				return ;
			}


			pProcess->ClearConCurrentTable();
		}

		void Undo(){}
	};
	
}

