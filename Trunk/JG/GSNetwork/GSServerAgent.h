#pragma once

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

	bool Add(GSCLIENT *pObj,DWORD Key);
	bool Del(DWORD Key);

	GSCLIENT* Search(DWORD Id);

	std::map<DWORD,GSCLIENT*> m_ServerMap;
};

}	

#define AGT_SVR Singleton<GSServerAgent>::Instance()