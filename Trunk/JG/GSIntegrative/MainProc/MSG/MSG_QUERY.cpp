#include "stdafx.h"
#include "MSG_QUERY.h"
#include "../NetworkSession/NetworkSession.h"
#include "ICommand.h"
#include "../DBProxy/DBJob/RequestCharInfo.h"
#include "../DBProxy/DBProcess.h"

#include "MSG_QUERY_RES.h"
#include "../Constructor/GSMainProcess.h"
#include "../Allocator/GSAllocator.h"
#include "../DBProxy//DBProcess.h"
#include "../DBProxy/DBProcessContainer.h"
#include "../GSPacket/protocol.h"
#include "../IOCP/ServerIOCP.h"

MSG_QUERY::MSG_QUERY()
{
}

MSG_QUERY::~MSG_QUERY()
{

}

void MSG_QUERY::Execute(LPVOID Param)
{
	DBPROCESS_PTR pProcess = DBPROCESSCONTAINER.Search(Type);
	if(pProcess == NULL)
		return;

	if(pSession != NULL)
	{
		switch(MsgId)
		{
		case _PROTOCOL_DBA::DBP_GET_CHAR_INFO:
			{
				RequestCharInfo *pData = (RequestCharInfo *)pRequst;

				pData->SetId(_PROTOCOL_DBA::DBP_GET_CHAR_INFO_RE);
				
				int Result = pProcess->ChooseCharacter(pData->Account.c_str(),pData);

				boost::shared_ptr<MSG_QUERY_RES>	    pRes = ALLOCATOR.Create<MSG_QUERY_RES>(); //(new MSG_QUERY_RES );//			

				pRes->pHandler	= GSServer::CServerIOCP::GetServerIOCP().GetSessionMgr();
				pRes->Type		= GSServer::GSMainProcess::GSMainProcess::MSG_TYPE_USER;
				pRes->SubType	= GSServer::CNetWorkSessionMgr::CNetWorkSessionMgr::USER_EVNT_TYPE::ONQUERY_RES;
				pRes->MsgId		= _PROTOCOL_DBA::DBP_GET_CHAR_INFO_RE;
				pRes->pResponse = pData;
				pRes->pSession  = pSession;
				pRes->Result    = Result;
				GSServer::GSMainProcess::GetMainProc()->RegisterCommand(pRes);	
			}
			break;

		case _PROTOCOL_DBA::DBP_REQUEST_CHANNEL_LIST:
			{
				RequestChannelList *pData = (RequestChannelList *)pRequst;

				memset(pData,0,sizeof(RequestChannelList));

				int Result = pProcess->ReqChannelList(pData);

				pData->SetId(_PROTOCOL_DBA::DBP_REQUEST_CHANNEL_LIST_RE);

				boost::shared_ptr<MSG_QUERY_RES>	    pRes = ALLOCATOR.Create<MSG_QUERY_RES>(); // (new MSG_QUERY_RES)	 ;

				pRes->pHandler	= GSServer::CServerIOCP::GetServerIOCP().GetSessionMgr();
				pRes->Type		= GSServer::GSMainProcess::GSMainProcess::EVT_TYPE::MSG_TYPE_USER;
				pRes->SubType	= GSServer::CNetWorkSessionMgr::CNetWorkSessionMgr::USER_EVNT_TYPE::ONQUERY_RES;
				pRes->MsgId		= _PROTOCOL_DBA::DBP_REQUEST_CHANNEL_LIST_RE;
				pRes->pResponse = pData;
				pRes->pSession  = pSession;
				pRes->Result    = Result;
				GSServer::GSMainProcess::GetMainProc()->RegisterCommand(pRes);	
			}
			break;
		}
	}

}
