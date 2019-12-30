#pragma once

#include "IHandler.h"
#include "./IMessage.h"
#include "ICommand.h"

#include <concurrent_queue.h>

#define MAX_QUUEUE_SIZE 6000

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

	//GSQueue<IMessage>  m_JobList[255];
	Concurrency::concurrent_queue<IMessagePtr> m_JobList[MAX_QUUEUE_SIZE];

	GSProactorImpl		*m_ProactorImpl;

	friend unsigned int __stdcall DistributionThread(LPVOID parameter);
	friend unsigned int __stdcall ExecuteThread(LPVOID parameter);

	HANDLE				m_InputJobEvt;
	HANDLE				m_ExecuteJobEvt[MAX_QUUEUE_SIZE];

	CRITICAL_SECTION m_Lock;
};

}	}

