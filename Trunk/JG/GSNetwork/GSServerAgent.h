#pragma once

#include <concurrent_unordered_map.h>

#include "IManager.h"
#include "GSServerAgentContainer.h"
#include "SingleTonHolder.h"
#include "GSClient.h"

namespace GSNetwork	{	
		
class GSServerAgent : public IServerContainer , public Singleton<GSServerAgent>
{
public:
	GSServerAgent(void);
	~GSServerAgent(void);

	bool Add(GSCLIENT_PTR pObj,DWORD Key);
	bool Del(DWORD Key);

	GSCLIENT_PTR Search(DWORD Id);

	concurrency::concurrent_unordered_map<DWORD, GSCLIENT_PTR> m_ServerMap;
};

}	

#define AGT_SVR Singleton<GSServerAgent>::Instance()