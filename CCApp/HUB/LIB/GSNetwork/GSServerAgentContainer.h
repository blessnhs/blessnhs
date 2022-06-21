#pragma once

#include "GSClient.h"

namespace GSNetwork	{	
	
class IServerContainer
{
public:
	IServerContainer()	 {}
	~IServerContainer() {}

	virtual bool Add(GSCLIENT_PTR pObj,DWORD Key) = 0;
	virtual bool Del(DWORD Key) = 0;

	virtual GSCLIENT_PTR Search(DWORD Id) = 0;
};

}	