#include "stdafx.h"
#include "GSJGServer.h"
#include "GSJGPlayerProcess.h"

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
#include "common.h"

GSJGServer::GSJGServer()
{

}

GSJGServer::~GSJGServer()
{

}


BOOL GSJGServer::Initialize()
{
	INI.Load(_T("./FrontSystem.ini"));

	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;


	GSArgument _arg;

	_arg.m_AliveTime = 300; //sec
	_arg.m_BindPort  = INI.m_Port;
	_arg.m_DBThreadCnt = dwThreadNum;
	_arg.m_LogicThreadCnt = 1;
	_arg.m_MaxClient = INI.m_MaxUser;
	_arg.m_Naggle = false;

	Create(_arg);
	SetHandler<GSJGPlayerProcess>();

	printf("ServerName [ GSJGServer ] Port [ %d ] MaxUser [ %d ]\n",INI.m_Port,INI.m_MaxUser);

	return TRUE;
}

BOOL GSJGServer::Disconnect(GSCLIENT *pSession)
{
	if(pSession != NULL)
	{
		if(pSession->GetType() ==_SERVER_)
		{
			printf("called player mgr reset\n");

			PLAYERMGR.DeleteAllBySession(pSession);
		}
		else if(pSession->GetType() == _PLAYER_)
		{
			if(pSession->GetDBStamp() > 0) //남아 있는 db작업이 있다.
			{
				if((pSession->GetDBStampTime()+1000*300) > GetTickCount())		//최대 5분 동안만 기다린다.
				{
					pSession->SetWillBeTerminated(TRUE);
				}
				printf("remain dbstamp %S %d\n",pSession->m_LoginContext.account,pSession->GetDBStamp());

				return FALSE;
			}

			if(pSession->m_LoginContext.IsSendedAuth == true ) //gs로 부터 아직 인증 패킷을 받지 못햇다.
			{
				printf("not complete auth packet %S\n",pSession->m_LoginContext.account);

				if((pSession->m_LoginContext.Time + 5000) < GetTickCount())
					pSession->m_LoginContext.IsSendedAuth = false;

				pSession->SetWillBeTerminated(TRUE);
				return FALSE;
			}

			GSFrontServer::Player::PlayerPtr pPlayer= PLAYERMGR.Search(pSession->GetPair());
			if(pPlayer != NULL)
			{
				PLAYERMGR.Del(pPlayer);

				//채널에 입장했다면 뺀다.
				CHANNEL_PTR ChannelPtr = CHANNELMGR.Search(0);
				if(ChannelPtr != NULL)
				{
					ChannelPtr->m_PlayerContainer.Del(pPlayer);

					ROOM_PTR pRoom = ChannelPtr->m_RoomContainer.Search(pPlayer->m_RoomNumber);
					if(pRoom)
					{
						pRoom->RemovePlayer(pPlayer);
						if(pRoom->GetCurrPlayer() == 0)
							ChannelPtr->m_RoomContainer.Del(pRoom);
					}
				}
			}

			if(wcslen(pSession->m_LoginContext.account) > 3)
			{
				DECLARE_JSON_WRITER
				ADD_JSON_MEMBER("ChannelId",0)

				int _id =  pSession->GetId();
				ADD_JSON_MEMBER("SessionId", _id)

				wstring _account = pSession->m_LoginContext.account;
				ADD_JSON_WSTR_MEMBER("Account",_account)

				END_JSON_MEMBER(UNITY.GetTCPSocket(),ID_FG_PKT_CLOSE_PLAYER_REQ)
			

				//로그아웃 쿼리를 날린다.
				ClosePlayerPtr pRequest =  ALLOCATOR.Create<ClosePlayer>(); //new ClosePlayer;// =
				pRequest->AccountId   = pSession->m_LoginContext.account;
				PROC_REG_QUERY_JOB(pRequest,DBP_REQUEST_CLOSE_PLAYER,pSession,GSFrontServer::MSG_PLAYER_QUERY,ClosePlayer)
			}
			else
				printf("account id short %S",pSession->m_LoginContext.account);


		}

		//printf("Disconnected socket  %d %S\n",pSession->GetSocket(),pSession->m_LoginContext.account);	
	}
	
	return TRUE;
}

VOID GSJGServer::Accept(GSCLIENT *object)
{

}
