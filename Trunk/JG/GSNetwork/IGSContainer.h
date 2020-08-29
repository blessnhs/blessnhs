#pragma once

#include "GSObject.h"

typedef GSNetwork::GSObject::GSObject GSObject;

template<class T>
class IContainer
{
public:
	IContainer(void)  {}
	~IContainer(void) {}

	virtual bool Add(T &pObj) = 0;
	virtual bool Del(T &pObj) = 0;
	virtual T Search(INT64 Id) = 0;
};


