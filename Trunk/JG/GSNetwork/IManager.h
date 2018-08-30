#pragma once

#include "GSObject.h"
#include "IFactory.h"
#include "IGSContainer.h"
#include "IProcess.h"
#include "IResponse.h"

template<class T>
class IManager : public IFactory<T> ,public IContainer<T>,	public IResponse
{
public:
	IManager(void)  {}
	~IManager(void) {}

	virtual void OnEvt(LPVOID Arg) = 0;
	virtual T &Get(DWORD Id) = 0;

};

