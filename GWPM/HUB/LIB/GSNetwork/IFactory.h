#pragma once

template<class T>
class IFactory
{
public:
	IFactory(void)   {}
	~IFactory(void)  {}

	virtual T &Create(WORD Type) = 0;
	virtual void Destroy(T &obj) = 0;
};

