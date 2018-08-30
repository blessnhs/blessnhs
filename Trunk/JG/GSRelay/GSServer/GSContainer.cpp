#include "StdAfx.h"
#include "GSContainer.h"

//#include "./ServerIOCP_UDP.h"
#include "../GSPacket/protocol.h"

namespace GSServer	{	namespace GSContainer	{
/*
	GSContainer::GSContainer(void)
	{

	}

	GSContainer::~GSContainer(void)
	{
	}

	bool GSContainer::Add(GSS &pObj)
	{
		if(m_List.find(pObj.m_Stock.Id) != m_List.end())
		{

		}

		m_List[pObj.m_Stock.Id] = &pObj;


		return TRUE;
	}

	bool GSContainer::Del(GSS &pObj)
	{
		std::map<DWORD,GSS*>::iterator iter = m_List.find(pObj.m_Stock.Id);
		if(iter != m_List.end())
		{
			m_List.erase(iter);
		}
		else 
			return FALSE;

		return TRUE;
	}

	GSS GSContainer::Search(DWORD Id)
	{
		return GSS();
	}

	void GSContainer::Initialize()
	{
		RequestChannelList* pRequester = ALLOCATOR.m_RequestChannelListPool.malloc();	

		boost::shared_ptr<MSG_QUERY>		pPlayerQuery
			= ALLOCATOR.Create<MSG_QUERY>(Type2Type<MSG_QUERY>());

		pPlayerQuery->pHandler= IOCP.GetSessionMgr();
		pPlayerQuery->Type	= GSServer::GSMainProcess::GSMainProcess::EVT_TYPE::MSG_TYPE_DB_1;
		pPlayerQuery->SubType = GSServer::CNetWorkSessionMgr::CNetWorkSessionMgr::USER_EVNT_TYPE::ONQUERY;
		pPlayerQuery->MsgId = _PROTOCOL_DBA::DBP_REQUEST_CHANNEL_LIST;
		pPlayerQuery->pRequst = (LPVOID)pRequester;
		pPlayerQuery->pSession= (IResponse*)IOCP.GetSessionMgr();

		GSServer::GSMainProcess::GetMainProc()->RegisterCommand(pPlayerQuery);
	}


	void GSContainer::TryConnect()
	{
		std::map<DWORD,GSS*>::iterator iter = m_List.begin();

		while(iter != m_List.end())
		{
			iter->second->FrontConnect();
			iter++;
		}
	}*/

}	}