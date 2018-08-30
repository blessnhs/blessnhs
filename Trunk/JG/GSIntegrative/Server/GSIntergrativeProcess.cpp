#include "StdAfx.h"
#include "GSIntergrativeProcess.h"

#include "GSSerialize.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "../GSPacket/protocol.h"
#include "../GSPacket/Front.h"
#include "../PLAYER/PlayerManager.h"

GSIntergrativeProcess::GSIntergrativeProcess(void)
{
	ADD_NET_FUNC(GSIntergrativeProcess,ID_GF_PKT_REGIST_REQ,REGIST_FRONT_SVR);
	ADD_NET_FUNC(GSIntergrativeProcess,ID_FG_PKT_REGIST_PLAYER_REQ,REGIST_PLAYER);
	ADD_NET_FUNC(GSIntergrativeProcess,ID_FG_PKT_CLOSE_PLAYER_REQ,CLOSE_PLAYER);
}


GSIntergrativeProcess::~GSIntergrativeProcess(void)
{
}


VOID GSIntergrativeProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}

VOID GSIntergrativeProcess::REGIST_FRONT_SVR(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Id",Id)

	printf("Regist Server Success %d\n",Id);

	pOwner->SetType(FRONT_SERVER);

	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result",0)
	END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FG_PKT_REGIST_RES)

}

VOID GSIntergrativeProcess::REGIST_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("ChannelId",ChannelId)
		GET_JSON_INT_MEMBER("Index",Index)
		GET_JSON_INT_MEMBER("SessionId",SessionId)
		GET_JSON_WSTR_MEMBER("Account",Account)
		GET_JSON_WSTR_MEMBER("CharName",CharName)

		if(Account.size() < 3)
		{
			return ;
		}

		PlayerPtr	pPlayerPtr = PLAYERMGR.Search(Account);
		if(pPlayerPtr != NULL)
		{
			pPlayerPtr->SetId(Index);
			pPlayerPtr->CharName    = CharName;
			pPlayerPtr->AccountName = Account;

			pPlayerPtr->m_WillBeTerminated	= FALSE;
			pPlayerPtr->m_DBStamp  = 0;
			pPlayerPtr->FrontSession = pOwner;
			pPlayerPtr->m_SessionId  = SessionId;

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",0)
				ADD_JSON_MEMBER("SessionId",SessionId)
				ADD_JSON_MEMBER("ChannelId",ChannelId)
				ADD_JSON_MEMBER("m_Index",Index)
				ADD_JSON_WSTR_MEMBER("CharName",CharName)
				ADD_JSON_WSTR_MEMBER("Account",Account)

				END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_GF_PKT_REGIST_PLAYER_RES)

				printf("REGIST_PLAYER %S exist player\n",Account.c_str());
			return ;
		}

		PlayerPtr	pPlayer = PLAYERMGR.GetPlayerPool().alloc();

		pPlayer->SetId(Index);
		pPlayer->CharName    = CharName;
		pPlayer->AccountName = Account;

		pPlayer->m_WillBeTerminated	= FALSE;
		pPlayer->m_DBStamp  = 0;
		pPlayer->FrontSession = pOwner;
		pPlayer->m_SessionId  = SessionId;
		bool Result = PLAYERMGR.Add(pPlayer);
		if( Result == FALSE)
		{
			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Result",1)
				ADD_JSON_MEMBER("SessionId",SessionId)
				ADD_JSON_MEMBER("ChannelId",ChannelId)
				ADD_JSON_MEMBER("m_Index",Index)
				ADD_JSON_WSTR_MEMBER("CharName",CharName)
				ADD_JSON_WSTR_MEMBER("Account",Account)

				END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_GF_PKT_REGIST_PLAYER_RES)

				printf("PLAYERMGR.Add(pPlayer) failed %S\n",Account.c_str());
			//pPlayer->m_WillBeTerminated	= TRUE;
			return ;
		}

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Result",0)
			ADD_JSON_MEMBER("SessionId",SessionId)
			ADD_JSON_MEMBER("ChannelId",ChannelId)
			ADD_JSON_MEMBER("m_Index",Index)
			ADD_JSON_WSTR_MEMBER("CharName",CharName)
			ADD_JSON_WSTR_MEMBER("Account",Account)

			END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_GF_PKT_REGIST_PLAYER_RES)

}

VOID GSIntergrativeProcess::CLOSE_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("ChannelId",ChannelId)
	GET_JSON_INT_MEMBER("SessionId",SessionId)
	GET_JSON_WSTR_MEMBER("Account",Account)
	//GET_JSON_WSTR_MEMBER("CharName",CharName)

	PlayerPtr pPlayer = PLAYERMGR.SearchBySession(SessionId);
	if(pPlayer != NULL)
	{
		if(pPlayer->m_DBStamp > 0) //남아 있는 db작업이 있다.
		{
			pPlayer->m_WillBeTerminated = TRUE;
			printf("db job exist\n");
			return ;
		}

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Result",0)
		ADD_JSON_MEMBER("SessionId",SessionId)
		ADD_JSON_MEMBER("ChannelId",ChannelId)
		wstring charname;
		ADD_JSON_WSTR_MEMBER("CharName",charname)
		ADD_JSON_WSTR_MEMBER("Account",Account)

		GSCLIENT *pSession = (GSCLIENT *)this->pOwner;
		END_JSON_MEMBER(pSession->GetTCPSocket(),ID_GF_PKT_CLOSE_PLAYER_RES)


		PLAYERMGR.Del(pPlayer);

		//printf("CLOSE_PLAYER %S\n",Account.c_str());
	}
	else	//예외처리 
	{

	}



}

VOID GSIntergrativeProcess::ACCESS_GAMESVR(LPVOID Data,DWORD Length)
{
}