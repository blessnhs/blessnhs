#pragma once

template<class T>
class IProcess
{
public:
	IProcess(void){}
	~IProcess(void){}

	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol) = 0;

	T *pOwner;
};

