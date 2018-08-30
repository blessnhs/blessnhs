#include "StdAfx.h"
#include "MSG_PLAYER_TIME.h"

#include "ICommand.h"
#include "GSPacketTCP.h"
#include "IProcess.h"


namespace GSNetwork	{


MSG_PLAYER_TIME::MSG_PLAYER_TIME()
{

}

MSG_PLAYER_TIME::~MSG_PLAYER_TIME()
{

}

void MSG_PLAYER_TIME::Execute(LPVOID Param)
{
	pServer->GetClientMgr().CheckAliveTime();
}

}