#pragma once

template<class T>
class IHandler : public GSNetwork::GSObject::GSObject
{
public:
	IHandler() {}
	~IHandler(){}

	virtual void OnEvt(T Arg) = 0;
};