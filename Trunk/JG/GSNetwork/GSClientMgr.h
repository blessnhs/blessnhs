#pragma once

#include "GSClient.h"


namespace GSNetwork	{	namespace GSClientMgr	{

class GSClientMgr : public IHandler<IMessagePtr> ,  public GSGuard<GSClientMgr>
{
public:

	GSClientMgr();
	~GSClientMgr(void);

	virtual void OnEvt(IMessagePtr Arg) {}

	VOID CheckAliveTime();

	//연결되어있는 소켓 갯수
	int  GetActiveSocketCount();

	int IncClientId();

	BOOL Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID PServer);
	BOOL NewClient(SOCKET ListenSocket, LPVOID PServer);

	VOID End();

	GSCLIENT_PTR GetClient(int id);
	BOOL AddClient(GSCLIENT_PTR client);
	BOOL BookDelClient(int id);
	BOOL DelClient(int id);

	BOOL AddClientLoop(GSCLIENT_PTR client);
	BOOL DelClientLoop(int id);
	GSCLIENT_PTR GetClientLoop(int id);

	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_ReInsert_Queue;
	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_Remove_Queue;

	//클라이어언트 맵
	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_Clients;

	//루프를 위해 두벌 처리
	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_ClientsForLoop;

	void SetGSServer(LPVOID pServer)
	{
		m_GSServer = pServer;
	}

private:

	LPVOID m_GSServer;
	int    m_MaxClients;
};

}	}