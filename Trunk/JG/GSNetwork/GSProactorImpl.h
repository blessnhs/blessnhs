#pragma once

#include "IHandler.h"
#include "./IMessage.h"
#include "ICommand.h"

#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>

namespace GSFrames	{	namespace GSProactorImpl	{

class GSProactorImpl
{
public:
	GSProactorImpl(void);
	~GSProactorImpl(void);

	virtual bool Create(int InputTheadCnt);
	virtual bool Activate(int Type,int TCnt);
	virtual bool RegisterCommand(IMessagePtr msg);
	
	virtual bool Register(int Type,IMessagePtr Msg);
	virtual bool Remove();
	virtual bool Handle_Event(int ProcId);

protected:
	int				   m_InputThreadCnt;
	
	Concurrency::concurrent_queue<IMessagePtr> m_InputJobList;

	HANDLE			   m_hKillEvent;	
	HANDLE			   m_hThread;

	concurrency::concurrent_unordered_map<int, Concurrency::concurrent_queue<IMessagePtr>*> m_JobMap;
	concurrency::concurrent_unordered_map<int, HANDLE > m_ExecuteJobEvt;
	GSProactorImpl		*m_ProactorImpl;

	friend unsigned int __stdcall DistributionThread(LPVOID parameter);
	friend unsigned int __stdcall ExecuteThread(LPVOID parameter);

	HANDLE				m_InputJobEvt;

	CRITICAL_SECTION m_Lock;
};

}	}

