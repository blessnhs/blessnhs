#pragma once

#include <boost/make_shared.hpp>

class GSClient;

template<class T>
class IProcess
{
public:
	IProcess(void){}
	~IProcess(void){}

	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol, boost::shared_ptr<GSClient> pClient) = 0;

	int pOwnerId;
};

