#include "stdafx.h"
#include "ProxyContainer.h"
#include "../server/hubProcess.h"

ProxyHubContainer::ProxyHubContainer(void)
{

}

ProxyHubContainer::~ProxyHubContainer(void)
{
}

bool ProxyHubContainer::Add(GSCLIENT_PTR& pObj)
{
	int id = pObj->GetId();

	m_Proxy_Map[pObj->GetId()] = pObj;

	return TRUE;
}

bool ProxyHubContainer::Del(GSCLIENT_PTR& pObj)
{
	int id = pObj->GetId();

	m_Proxy_Map[pObj->GetId()] = NULL;

	return TRUE;
}

GSCLIENT_PTR ProxyHubContainer::Search(INT64 Id)
{
	auto iter = m_Proxy_Map.find(Id);
	if (iter != m_Proxy_Map.end())
	{
		return iter->second;
	}

	return GSCLIENT_PTR();
}

GSCLIENT_PTR ProxyHubContainer::GetHub(unsigned long id)
{
	std::vector< GSCLIENT_PTR> vt;

	for each (auto proxy in m_Proxy_Map)
	{
		if (proxy.second == NULL)
			continue;

		if (proxy.second->GetConnected() == false)
			continue;

		vt.push_back(proxy.second);
	}

	if(vt.size() == 0)
		return GSCLIENT_PTR();
	else
	{
		return vt[id % vt.size()];
	}
}


void ProxyHubContainer::Create(INT Count)
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)&SERVER;

	for (int i = 0; i < Count; i++)
	{
		auto pClient = pServer->GetClientMgr().NewClient2();
		if (pClient == NULL)
		{
			SYSLOG().Write(" ProxyHubContainer::Initialize failed...1 \n");
			break;
		}


		Add(pClient);

		pClient->SetHandler< HubProcess>();
	}

	TryConnect();

}


void ProxyHubContainer::TryConnect()
{
	GSServer::GSServer* pServer = (GSServer::GSServer*)&SERVER;

	for each(auto proxy in m_Proxy_Map)
	{
		GSCLIENT_PTR pClient = proxy.second;
		if (pClient == NULL)
			continue;

		if (pClient->GetConnected() == TRUE)
			continue;


		if (!pClient->GetTCPSocket()->Connect2(INI.HubIp, INI.HubPort))
		{
			SYSLOG().Write(" ProxyHubContainer::Initialize failed...2 \n");
			break;
		}

		pClient->SetConnected(true);

		int id = pClient->GetId();
		if (!pServer->RegIocpHandler(pClient->GetTCPSocket()->GetSocket(), reinterpret_cast<ULONG_PTR>(&id)))
		{
			SYSLOG().Write(" ProxyHubContainer::Initialize failed...3 \n");
			Del(pClient);

			SERVER.Close(pClient->GetTCPSocket()->GetSocket());
			break;
		}

		if (!pClient->GetTCPSocket()->Initialize())
		{
			SYSLOG().Write(" ProxyHubContainer::Initialize failed...4 \n");
			Del(pClient);
			SERVER.Close(pClient->GetTCPSocket()->GetSocket());

			break;
		}

		if (!pClient->InitializeReadForIocp())
		{
			SYSLOG().Write(" ProxyHubContainer::Initialize failed...5 \n");
			Del(pClient);
			SERVER.Close(pClient->GetTCPSocket()->GetSocket());

			break;
		}
	}
}

