#pragma once

namespace GSNetwork	{	namespace IClientSession	{

class IClientSession
{
public:
	IClientSession(void);
	~IClientSession(void);

	virtual void OnSend(WORD MainId,WORD SubId,char *Data,WORD Length) = 0;
	virtual void OnRecv(DWORD Length) = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnConnect() = 0;
};

}	}