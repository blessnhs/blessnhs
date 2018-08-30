#include "StdAfx.h"

#include "GSConstructor.h"

#include "../DBProxy/DBProcessContainer.h"
#include "../Channel/ChannelContainer.h"
#include "MiniDump.h"
#include "../GSPacket/Front.h"
#include "../PLAYER/Message/MSG_PLAYER_QUERY.h"
#include "../../GSPacket/protocol.h"
#include "../LOG/FrontLog.h"
#include "../Command/Command.h"
#include "../ServerAgent/GSServerAgent.h"

#include "GSMainProcess.h"
#include "GSAllocator.h"

namespace GSFrontServer	{	namespace GSConstructor	{

GSConstructor::GSConstructor(void)
{
	JGSERVER.Initialize();

	CHANNELMGR.Initialize();
	
	DBPROCESSCONTAINER.Initialize();

	//·Î±×¾Æ¿ô Äõ¸®¸¦ ³¯¸°´Ù.
	ClearConcurrentPtr pRequest  = ALLOCATOR.Create<ClearConcurrent>(); 
//	PROC_REG_QUERY_JOB(pRequest,DBP_REQUEST_CLEAR_CONCURRENT,NULL,MSG_PLAYER_QUERY,ClearConcurrent);
	{
		boost::shared_ptr<MSG_PLAYER_QUERY<ClearConcurrent>>		PLAYER_MSG = ALLOCATOR.Create<MSG_PLAYER_QUERY<ClearConcurrent>>();
		PLAYER_MSG->pRequst		= NULL;
		PLAYER_MSG->Type		= MSG_TYPE_DB_1;
		PLAYER_MSG->SubType		= ONQUERY;
		PLAYER_MSG->MsgId		= DBP_REQUEST_CLEAR_CONCURRENT;
		MAINPROC.RegisterCommand(PLAYER_MSG);
	}
	UNITY.Connect();
	
	FrontCommand _Command;

	while(TRUE)
	{
		WCHAR command[256];
		wscanf(_T("%s"),command);

		_Command.Execute(command);
	}
}


GSConstructor::~GSConstructor(void)
{
}

}	}