#pragma once

#include "../NetworkSession/NetworkSession.h"
#include "ICommand.h"
#include "../DBProxy/DBJob/RequestCharInfo.h"
#include "../DBProxy/DBProcess.h"
#include "MSG.h"
#include "../player/PlayerManager.h"
#include "../Allocator/GSAllocator.h"

class MSG_QUERY_RES :  public SvrMSG
{
public:
	MSG_QUERY_RES() {}
	~MSG_QUERY_RES(){}

	IResponse		*pSession;
	LPVOID			pResponse;
	WORD			Result;

	DWORD			MsgId;

	void Execute(LPVOID Param)
	{
		if(pSession != NULL)
		{
			pSession->OnResponse(pResponse);

			GSServer::Player::Player *pPlayer = (GSServer::Player::Player *)pSession;
			pPlayer->m_DBStamp--;

			
		}

	}
	void Undo(){}
};
