#pragma once

#include "GSClient.h"

namespace GSNetwork	{	namespace GSClientMgr	{

class GSClientMgr : public IHandler<IMessagePtr>
{
public:

	GSClientMgr(void);
	~GSClientMgr(void);

	virtual void OnEvt(IMessagePtr Arg) {}

	VOID CheckAliveTime();
	int  GetActiveSocketCount();

	BOOL Begin(SOCKET ListenSocket,WORD MaxClients,LPVOID PServer);
	VOID End();



	std::vector<GSClient::GSClient*> m_Clients;
};

}	}