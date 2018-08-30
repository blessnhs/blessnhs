#pragma once

#include "IHandler.h"
#include "./IMessage.h"
#include "ICommand.h"

#include <concurrent_queue.h>


namespace GSFrames	{	namespace GSProactorImpl	{

class GSProactorImpl
{
public:
	GSProactorImpl(void);
	~GSProactorImpl(void);

	virtual bool Create(int InputTheadCnt);
	virtual bool Activate(int Type,int TCnt);
	virtual bool RegisterCommand(IMessagePtr msg);
	
	virtual bool Register(BYTE Type,IMessagePtr Msg);
	virtual bool Remove();
	virtual bool Handle_Event(BYTE ProcId);

protected:
	int				   m_InputThreadCnt;
	
	Concurrency::concurrent_queue<IMessagePtr> m_InputJobList;

	HANDLE			   m_hKillEvent;	
	HANDLE			   m_hThread;

	//GSQueue<IMessage>  m_JobList[255];
	Concurrency::concurrent_queue<IMessagePtr> m_JobList[USHRT_MAX];

	GSProactorImpl		*m_ProactorImpl;

	friend unsigned int __stdcall DistributionThread(LPVOID parameter);
	friend unsigned int __stdcall ExecuteThread(LPVOID parameter);

	HANDLE				m_InputJobEvt;
	HANDLE				m_ExecuteJobEvt[USHRT_MAX];

	CRITICAL_SECTION m_Lock;
};

}	}

