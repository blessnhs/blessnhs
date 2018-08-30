#pragma once

#include "../PLAYER/NetworkSession.h"

namespace GSFrontServer	{	 namespace IFieldServerContainer	{
	
class IFieldServerContainer
{
public:
	IFieldServerContainer()	 {}
	~IFieldServerContainer() {}

	virtual bool Add(SESSION *pObj,DWORD Key) = 0;
	virtual bool Del(DWORD Key) = 0;

	virtual SESSION* Search(DWORD Id) = 0;
};

}	}