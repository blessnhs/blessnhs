#pragma once

#include "GSClient.h"
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>

namespace GSNetwork	{	namespace GSClientMgr	{

class GSClientMgr : public IHandler<IMessagePtr> ,  public GSGuard<GSClientMgr>
{
public:

	GSClientMgr();
	~GSClientMgr(void);

	virtual void OnEvt(IMessagePtr Arg) {}

	VOID CheckAliveTime();
	int  GetActiveSocketCount();

	int IncClientId();

	BOOL Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID PServer);
	BOOL NewClient(SOCKET ListenSocket, LPVOID PServer);

	VOID End();

	GSCLIENT_PTR GetClient(int id);
	BOOL AddClient(GSCLIENT_PTR client);
	BOOL BookDelClient(int id);
	BOOL DelClient(int id);


	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_ReInsert_Queue;
	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_Remove_Queue;
	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_Clients;

	void SetGSServer(LPVOID pServer)
	{
		m_GSServer = pServer;
	}

private:

	LPVOID m_GSServer;
};

}	}