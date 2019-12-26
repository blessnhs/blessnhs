#pragma once

#include "GSObject.h"
#include "IHandler.h"
#include "./IMessage.h"
#include <concurrent_queue.h>

namespace GSFrames	{	namespace GSReactorImpl	{

class GSReactorImpl
{
public:
	GSReactorImpl(void);
	~GSReactorImpl(void);

	virtual int Initialize();
	virtual void Terminate();
	
	virtual bool Register(IMessagePtr Msg);
	virtual bool Remove(IMessagePtr Msg);

	IMessagePtr GetMsg();

	Concurrency::concurrent_queue<IMessagePtr>  m_JobList;

	HANDLE				m_hKillEvent;	
	HANDLE				m_hThread;

	virtual void _HandleEvent();

	friend unsigned int __stdcall HandleEvent(LPVOID parameter);

};

}	}

