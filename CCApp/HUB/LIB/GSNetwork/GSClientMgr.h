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
	BOOL NewClient(int cnt = 1);

	//서버와의 연결을 위한 클라이언트 세션 생성
	GSCLIENT_PTR NewClient2();

	VOID End();

	GSCLIENT_PTR GetClient(int id);
	BOOL AddClient(GSCLIENT_PTR client);
	BOOL BookDelClient(int id);
	BOOL DelClient(int id);
	VOID AllCloseSocket();

	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_ReInsert_Queue;
	concurrency::concurrent_queue<GSCLIENT_PTR>				 m_Remove_Queue;

	//클라이어언트 맵
	concurrency::concurrent_unordered_map<int, GSCLIENT_PTR> m_Clients;

	void SetGSServer(LPVOID pServer)
	{
		m_GSServer = pServer;
	}

	void InsertRecycleId(int _id);
	int PopRecycleId();
private:

	LPVOID m_GSServer;
	int    m_MaxClients;

	//재사용 id
	concurrency::concurrent_queue<int>						m_Reycle_Id_Queue;

};

}	}