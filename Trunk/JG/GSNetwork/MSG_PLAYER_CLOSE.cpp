#include "StdAfx.h"
#include "MSG_PLAYER_CLOSE.h"

#include "GSSerialize.h"
#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSServer.h"

namespace GSNetwork	{


MSG_PLAYER_CLOSE::MSG_PLAYER_CLOSE()
{

}

MSG_PLAYER_CLOSE::~MSG_PLAYER_CLOSE()
{

}

void MSG_PLAYER_CLOSE::Execute(LPVOID Param)
{
	if(pClient != NULL)
	{
		printf("Disconnect socket %d %d\n", pClient->GetSocket(),pClient->GetId());
	
		GSServer::GSServer* pServer = (GSServer::GSServer*)pClient->m_GSServer;

		pServer->Disconnect(pClient);
	}
	else
	{
		printf("MSG_PLAYER_CLOSE!!!!!!!!!!  %d %d\n",pClient->GetSocket(),pListen->GetSocket());
	}
}


}