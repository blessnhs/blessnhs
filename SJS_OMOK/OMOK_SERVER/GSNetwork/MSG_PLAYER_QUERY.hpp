
#include "MSG_PLAYER_QUERY.h"

#include "MSG_PLAYER_QUERY_RES.h"

#include <boost/shared_ptr.hpp>
#include "GSClient.h"
#include "DBProcess.h"
#include "DBProcessContainer.h"
#include "GSServer.h"
#include "GSMainProcess.h"
#include "GSAllocator.h"

namespace GSNetwork	{

	template<class T>
	class MSG_PLAYER_QUERY : public IMessage
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
	class MSG_PLAYER_QUERY<RequestPlayerAuth> : public IMessage
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		GSCLIENT		*pSession;
		DBPROCESS_PTR   pProcess;

		boost::shared_ptr<RequestPlayerAuth> pRequst;

		void Execute(LPVOID Param)
		{
			pProcess = DBPROCESSCONTAINER.Search(Type);
			if(pProcess == NULL)
				return ;

			boost::shared_ptr<RequestPlayerAuth> res = pRequst;
			if(pSession != NULL)
			{
				if(pSession->GetConnected() == false)
				{
					return ;
				}

				res->Index = 0;

				res->Index = pProcess->ProcedureUserLogin(res->Account.c_str(),res->SessionKey.c_str(),0);

				if(res->Index != 0)
					res->Result = pProcess->SelectCharacterInfo(res->Account.c_str(),*res);
				else
					SYSLOG().Write("2.MSG_PLAYER_QUERY _PROTOCOL_DBA::DBP_GET_LOGIN_INFO index is  %s %s\n",res->Account.c_str(),res->SessionKey.c_str());

				PROC_REG_QUERY_RES_JOB(res,pSession,res->Result,MSG_PLAYER_QUERY_RES,RequestPlayerAuth)

			}
	
		}

		void Undo(){}
	};
	/*
	template<>
	class MSG_PLAYER_QUERY<ClosePlayer> : public IMessage
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		SESSION			*pSession;
		DBPROCESS_PTR   pProcess;

		boost::shared_ptr<ClosePlayer> pRequst;

		void Execute(LPVOID Param)
		{
			pProcess = DBPROCESSCONTAINER.Search(Type);
			if(pProcess == NULL)
				return ;

					
			pProcess->ClosePlayer((WCHAR *)pRequst->AccountId.c_str());
		

		}
		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY<ClearConcurrent> : public IMessage
	{
	public:
		MSG_PLAYER_QUERY() { pSession = NULL; }
		~MSG_PLAYER_QUERY(){}

		DWORD			MsgId;

		SESSION			*pSession;
		DBPROCESS_PTR   pProcess;

		boost::shared_ptr<ClearConcurrent> pRequst;

		void Execute(LPVOID Param)
		{
			pProcess = DBPROCESSCONTAINER.Search(Type);
			if(pProcess == NULL)
				return ;


			pProcess->ClearConCurrentTable();
		}

		void Undo(){}
	};*/
	
}

