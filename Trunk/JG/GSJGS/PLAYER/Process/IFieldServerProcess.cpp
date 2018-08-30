#include "StdAfx.h"
#include "IFieldServerProcess.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/MainLogin.h"
#include "../../FieldServer/FiledServer.h"
#include "../Container/PlayerContainer.h"
#include "../../PLAYER/Container/Player.h"
#include "../Message/MSG_PLAYER_CLOSE.h"
#include "../../Allocator/GSAllocator.h"
#include "../../Constructor/GSMainProcess.h"
#include "../../IOCP/ServerIOCP.h"
#include "../Sender/Sender.h"
#include "../../Channel/ChannelContainer.h"
#include "GSSerialize.h"
#include "../../LOG/FrontLog.h"
#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "../Message/MSG_PLAYER_CLOSE.h"
#include "../Message/MSG_PLAYER_QUERY.h"

#include "../GSPacket/Protocol.h"

namespace GSFrontServer {	namespace IFieldServerProcess	{

IFieldServerProcess::IFieldServerProcess(void)
{
	ADD_NET_FUNC(IFieldServerProcess,ID_GF_PKT_REGIST_REQ,REGIST_FIELDSERVER)
	ADD_NET_FUNC(IFieldServerProcess,ID_GF_PKT_CLOSE_PLAYER_RES,CLOSE_PLAYER)
	ADD_NET_FUNC(IFieldServerProcess,ID_GF_PKT_REGIST_PLAYER_RES,REGISTER_PLAYER)
}


IFieldServerProcess::~IFieldServerProcess(void)
{
}

VOID IFieldServerProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}

VOID IFieldServerProcess::REGIST_FIELDSERVER(LPVOID Data,DWORD Length)
{

	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Id",Id)
	GET_JSON_INT_MEMBER("Type",Type)

	SESSION *pSession = (SESSION *)this->pOwner;

	SESSION *pSearchSession = FIELD_SVR.Search(Id);
	if(pSearchSession != NULL)		//이미 등록 된다.
	{
		FIELD_SVR.Del(Id);
	}

	pSession->SetType(SESSION::_FIELD_SERVER_);

	int Result = FIELD_SVR.Add((SESSION *)pSession,Id);
	
	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result",Result)
	END_JSON_MEMBER(pSession,ID_FG_PKT_REGIST_RES)
	
	printf("Accept Field Server Regist = %d Result = %d\n",Id,Result);
}

VOID IFieldServerProcess::CLOSE_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("SessionId",SessionId)
	GET_JSON_INT_MEMBER("Result",Result)
	GET_JSON_INT_MEMBER("ChannelId",ChannelId)
	GET_JSON_WSTR_MEMBER("Account",Account)
	GET_JSON_WSTR_MEMBER("CharName",CharName)

	printf("%S %d Result %d\n",Account.c_str(),SessionId,Result);

	//로그아웃 쿼리를 날린다.
	ClosePlayerPtr pRequest = ALLOCATOR_.Create<ClosePlayer>(); // new ClosePlayer;//
	pRequest->AccountId   = Account;
	PROC_REG_QUERY_JOB(pRequest,_PROTOCOL_DBA::DBP_REQUEST_CLOSE_PLAYER,NULL,MSG_PLAYER_QUERY,ClosePlayer)
}

VOID IFieldServerProcess::REGISTER_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("SessionId",SessionId)
	GET_JSON_INT_MEMBER("Result",Result)
	GET_JSON_INT_MEMBER("ChannelId",ChannelId)
	GET_JSON_INT_MEMBER("m_Index",m_Index)
	GET_JSON_WSTR_MEMBER("Account",Account)
	GET_JSON_WSTR_MEMBER("CharName",CharName)

	if(Result == 0)
	{
		SESSION *Session = IOCP.GetSessionMgr()->GetSession(SessionId);
		if(Session != NULL)
		{
			Session->m_LoginContext.IsSendedAuth = false;
			if(Session->GetConnected() == false)
			{
				printf("F3\n");

				Session->OnDisconnect();
				return ;
			}

			GSFrontServer::PlayerContainer::PlayerPtr pNewPlayer = PLAYERMGR.Create();
			pNewPlayer->Initialize();
			pNewPlayer->m_Char[0].SetName(CharName);
			pNewPlayer->m_Account.SetName(Account);
			pNewPlayer->SetId(m_Index);
			pNewPlayer->m_FieldServerId = pOwner->GetId();

			pNewPlayer->SetPair(SessionId);

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",FC_PKT_REGIST_RES::SUCCESS)

			wstring Name = CharName;
			ADD_JSON_WSTR_MEMBER("CharName",Name)
			END_JSON_MEMBER(Session,ID_FC_PKT_REGIST_RES)

			Session->SetPair(pNewPlayer->GetId());

			bool Result = PLAYERMGR.Add(pNewPlayer);

			if(Result == FALSE)
			{
				Session->OnDisconnect();

				printf("F3 %S %S\n",Session->m_LoginContext.account,Account);
				return ;
			}	
		
			{

				DECLARE_JSON_WRITER
				ADD_JSON_MEMBER("Result",FC_PKT_REGIST_RES::SUCCESS)
					wstring Name = CharName;
				ADD_JSON_WSTR_MEMBER("CharName",Name)
					END_JSON_MEMBER(Session,ID_FC_PKT_CHOOSE_CHAR_RES)
			}
		}
	
	}
	else		//실패면 접속 해재
	{
		printf("F5 %d\n",SessionId);
		SESSION *Session = IOCP.GetSessionMgr()->GetSession(SessionId);
		if(Session != NULL)
		{
			Session->m_LoginContext.IsSendedAuth = false;
			Session->OnDisconnect();
		}
	}

}


}	}