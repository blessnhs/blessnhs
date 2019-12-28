#pragma once

#include "GSClient.h"
#include <concurrent_unordered_map.h>

namespace GSNetwork	{	namespace GSClientMgr	{

class GSClientMgr : public IHandler<IMessagePtr> ,  public GSGuard<GSClientMgr>
{
public:

	GSClientMgr(void);
	~GSClientMgr(void);

	virtual void OnEvt(IMessagePtr Arg) {}

	VOID CheckAliveTime();
	int  GetActiveSocketCount();

	int IncClientId();

	BOOL Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID PServer);
	BOOL NewClient(SOCKET ListenSocket, LPVOID PServer);

	VOID End();

	GSCLIENT_PTR GetClient(int id);
	BOOL DelClient(int id);
	BOOL AddClient(GSCLIENT_PTR client);


	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_Clients;
};

}	}