#pragma once

#include "GSClient.h"
#include <concurrent_unordered_map.h>

namespace GSNetwork	{	namespace GSClientMgr	{

class GSClientMgr : public IHandler<IMessagePtr>
{
public:

	GSClientMgr(void);
	~GSClientMgr(void);

	virtual void OnEvt(IMessagePtr Arg) {}

	VOID CheckAliveTime();
	int  GetActiveSocketCount();

	int IncClientId();

	BOOL Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID PServer);
	BOOL NewClient(SOCKET ListenSocket, WORD Count, LPVOID PServer);

	VOID End();

	GSCLIENT_PTR GetClient(int id);
	BOOL DelClient(int id);


	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_Clients;
};

}	}