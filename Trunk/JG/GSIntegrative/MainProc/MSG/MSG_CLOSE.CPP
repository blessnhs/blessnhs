
#include "stdafx.h"
#include "MSG_CLOSE.h"
#include "../Player/PlayerManager.h"
#include "../Process/Adapt.h"

void MSG_CLOSE::Execute(LPVOID Param)
{
	if(pSession != NULL)
	{
		IProcess<GSServer::CNetworkSession::CNetworkSession> *pProcess = pSession->GetProcess();
		if(pProcess != NULL)
			delete pProcess;

		pSession->SetProcess(NULL);
	
		pSession->Recycle(pListen->GetSocket());

		if(pSession->GetType() == Adapt::FIELD_SERVER)
			PLAYERMGR.DestoryAll();
	
	}
	 
}
