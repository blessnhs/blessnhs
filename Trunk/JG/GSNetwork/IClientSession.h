#pragma once

#include <boost/make_shared.hpp>

class GSClient;

namespace GSNetwork	{	namespace IClientSession	{

class IClientSession
{
public:
	IClientSession(void);
	~IClientSession(void);

	virtual void OnSend(WORD MainId,WORD SubId,char *Data,WORD Length) = 0;
	virtual void OnRecv(DWORD Length, boost::shared_ptr<GSClient> Client) = 0;
	virtual void OnDisconnect(boost::shared_ptr<GSClient>) = 0;
	virtual void OnConnect() = 0;
};

}	}