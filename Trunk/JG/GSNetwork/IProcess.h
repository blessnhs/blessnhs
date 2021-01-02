#pragma once

class GSClient;

template<class T>
class IProcess
{
public:
	IProcess(void){}
	virtual  ~IProcess(void){}

	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol, boost::shared_ptr<GSClient> pClient) = 0;
	virtual VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) = 0;
	

	int pOwnerId;
};

