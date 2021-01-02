#pragma once

class GSClient;

namespace GSNetwork	{	namespace IClientSession	{

class IClientSession
{
public:
	IClientSession(void);
	virtual ~IClientSession(void);

	virtual void OnSend(WORD MainId,WORD SubId,char *Data,WORD Length) = 0;
	virtual void OnRecv(DWORD Length, boost::shared_ptr<GSClient> Client) = 0;
	virtual void OnDisconnect(boost::shared_ptr<GSClient>,bool isForce = false) = 0;
	virtual void OnConnect(boost::shared_ptr<GSClient>) = 0;
};

}	}