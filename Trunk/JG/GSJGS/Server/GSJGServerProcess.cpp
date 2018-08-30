#include "StdAfx.h"
#include "GSJGServerProcess.h"

#include "../GSPacket/Front.h"
#include "../GSPacket/MainLogin.h"
#include "../PLAYER/Message/MSG_PLAYER_QUERY.h"
#include "../PLAYER/Container/Player.h"
#include "../PLAYER/Container/PlayerContainer.h"
#include "../DBProxy/DBProcess.h"
#include "../GSPacket/Protocol.h"
#include "../room/RoomContainer.h"
#include "../Channel/ChannelContainer.h"
#include "GSSerialize.h"
#include "../LOG/FrontLog.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSMainProcess.h"
#include "GSAllocator.h"

#include "../DBProxy/DBJob/ClosePlayer.h"

#include "../room/RoomContainer.h"

GSJGServerProcess::GSJGServerProcess(void)
{
	ADD_NET_FUNC(GSJGServerProcess,ID_FG_PKT_REGIST_RES,REGIST_FIELDSERVER)
	ADD_NET_FUNC(GSJGServerProcess,ID_GF_PKT_CLOSE_PLAYER_RES,CLOSE_PLAYER)
	ADD_NET_FUNC(GSJGServerProcess,ID_GF_PKT_REGIST_PLAYER_RES,REGISTER_PLAYER)
}


GSJGServerProcess::~GSJGServerProcess(void)
{
}


VOID GSJGServerProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}
VOID GSJGServerProcess::REGIST_FIELDSERVER(LPVOID Data,DWORD Length)
{

	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Result",Result)

	GSCLIENT *pSession = (GSCLIENT *)this->pOwner;

	pSession->SetType(_SERVER_);

/*	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result",Result)
	END_JSON_MEMBER(pSession->GetTCPSocket(),ID_FG_PKT_REGIST_RES)*/

	printf("Accept Field Server Regist Result = %d\n",Result);
}

VOID GSJGServerProcess::CLOSE_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("SessionId",SessionId)
	GET_JSON_INT_MEMBER("Result",Result)
	GET_JSON_INT_MEMBER("ChannelId",ChannelId)
	GET_JSON_WSTR_MEMBER("Account",Account)
	GET_JSON_WSTR_MEMBER("CharName",CharName)

	//로그아웃 쿼리를 날린다.
	ClosePlayerPtr pRequest = ALLOCATOR.Create<ClosePlayer>(); // new ClosePlayer;//
	pRequest->AccountId   = Account;
	PROC_REG_QUERY_JOB(pRequest,_PROTOCOL_DBA::DBP_REQUEST_CLOSE_PLAYER,pOwner,GSFrontServer::MSG_PLAYER_QUERY,ClosePlayer)
}

VOID GSJGServerProcess::REGISTER_PLAYER(LPVOID Data,DWORD Length)
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
		GSCLIENT *Session = JGSERVER.GetClient(SessionId);
		if(Session != NULL)
		{
			Session->m_LoginContext.IsSendedAuth = false;
			if(Session->GetConnected() == false)
			{
				printf("F3\n");

				Session->Close();
				//Session->OnDisconnect();
				return ;
			}

			GSFrontServer::PlayerContainer::PlayerPtr pNewPlayer = PLAYERMGR.Create();
			pNewPlayer->Initialize();
			pNewPlayer->m_Char[0].SetName(CharName);
			pNewPlayer->m_Account.SetName(Account);
			pNewPlayer->SetId(m_Index);
			pNewPlayer->m_UnityServerId = pOwner->GetId();

			pNewPlayer->SetPair(SessionId);

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",FC_PKT_REGIST_RES::SUCCESS)
			ADD_JSON_MEMBER("Index",m_Index)

				wstring Name = CharName;
			ADD_JSON_WSTR_MEMBER("CharName",Name)
				END_JSON_MEMBER(Session->GetTCPSocket(),ID_FC_PKT_REGIST_RES)

				Session->SetPair(pNewPlayer->GetId());

			bool Result = PLAYERMGR.Add(pNewPlayer);

			if(Result == FALSE)
			{
				//Session->OnDisconnect();
				Session->Close();

				printf("F3 %S %S\n",Session->m_LoginContext.account,Account);
				return ;
			}	

			{

				DECLARE_JSON_WRITER
				ADD_JSON_MEMBER("Result",FC_PKT_REGIST_RES::SUCCESS)
					wstring Name = CharName;
				ADD_JSON_WSTR_MEMBER("CharName",Name)
					END_JSON_MEMBER(Session->GetTCPSocket(),ID_FC_PKT_CHOOSE_CHAR_RES)
			}
		}

	}
	else		//실패면 접속 해재
	{
		printf("F5 %d\n",SessionId);
		GSCLIENT *Session = JGSERVER.GetClient(SessionId);
		if(Session != NULL)
		{
			Session->m_LoginContext.IsSendedAuth = false;
			//Session->OnDisconnect();
			Session->Close();
		}
	}

}
