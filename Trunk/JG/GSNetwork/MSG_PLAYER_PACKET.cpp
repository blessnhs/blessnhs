#include "StdAfx.h"
#include "MSG_PLAYER_PACKET.h"

#include "ICommand.h"
#include "GSPacketTCP.h"
#include "ICommand.h"
#include "GSBuffer.h"


namespace GSNetwork	{


MSG_PLAYER_PACKET::MSG_PLAYER_PACKET()
{

}

MSG_PLAYER_PACKET::~MSG_PLAYER_PACKET()
{

}

void MSG_PLAYER_PACKET::Execute(LPVOID Param)
{
	if(pClient != NULL)
	{
		pClient->ProcPacket(pClient);
	}
}

}