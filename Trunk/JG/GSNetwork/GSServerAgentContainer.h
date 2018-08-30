#pragma once

#include "GSClient.h"

namespace GSNetwork	{	
	
class IServerContainer
{
public:
	IServerContainer()	 {}
	~IServerContainer() {}

	virtual bool Add(GSCLIENT *pObj,DWORD Key) = 0;
	virtual bool Del(DWORD Key) = 0;

	virtual GSCLIENT* Search(DWORD Id) = 0;
};

}	