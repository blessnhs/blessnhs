#include "StdAfx.h"
#include "GSProactorImpl.h"
#include "GSExecuteArgument.h"

namespace GSFrames	{	namespace GSProactorImpl	{

GSProactorImpl::GSProactorImpl(void)
{
}


GSProactorImpl::~GSProactorImpl(void)
{
	for each (auto job in m_JobMap)
	{
		if (job.second != NULL)
			delete job.second;
	}
}

bool GSProactorImpl::Create(int InputTheadCnt)
{
	m_hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_InputJobEvt	= CreateEvent(NULL, TRUE, FALSE, NULL);

	if (NULL == m_hKillEvent || NULL == m_InputJobEvt)
	{
		SYSLOG().Write("Create Kill Event failed (%d)\n", GetLastError());
		return 1;
	}


	unsigned int WorkerId = 0;
	for (int i = 0; i < InputTheadCnt; i++)
	{
		WorkerId = i;
		m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, DistributionThread, (LPVOID)this, 0, &WorkerId));
	}

	return TRUE;
}

bool GSProactorImpl::Activate(int Type,int TCnt)
{
	GSExecuteArgument *argument = new GSExecuteArgument();
	argument->m_ExecuteType		= Type;
	argument->m_ProcactorImpl   = this;

	unsigned int WorkerId = 0;
	for(int i=0;i<TCnt;i++)
	{
		argument->m_Id = WorkerId = i;
		m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ExecuteThread, (LPVOID)argument, 0, &WorkerId));
	}
	
	return TRUE;
}

bool GSProactorImpl::RegisterCommand(IMessagePtr msg)
{
	m_InputJobList.push(msg);
	SetEvent(m_InputJobEvt);

	return TRUE;
}

bool GSProactorImpl::Remove()
{
	return false;
}

bool GSProactorImpl::Register(int Type,IMessagePtr Msg)
{
	if (m_JobMap.find(Type) == m_JobMap.end())
	{
		m_JobMap[Type] = new Concurrency::concurrent_queue<IMessagePtr>();
		m_ExecuteJobEvt[Type] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	m_JobMap[Type]->push(Msg);


	SetEvent(m_ExecuteJobEvt[Type]);
	
	return TRUE;
}

unsigned int __stdcall DistributionThread(LPVOID parameter)
{
	GSProactorImpl *Owner = (GSProactorImpl*) parameter;

	while(TRUE)
	{
		if( WaitForSingleObject( Owner->m_hKillEvent, 1 ) == WAIT_OBJECT_0 ){ break; }
		
		IMessagePtr pJob;
		if(Owner->m_InputJobList.try_pop(pJob) == FALSE) 
		{
			Sleep(100);
			continue;
		}
	
		if(pJob != NULL)
		{
			Owner->Register(pJob->Type,pJob);
		}
		else
		{
			SYSLOG().Write("pJob is null");
		}

//		if(Owner->m_InputJobList.unsafe_size() == 0)
//			ResetEvent(Owner->m_InputJobEvt);
	}

	return 0;
}

unsigned int __stdcall ExecuteThread(LPVOID parameter)
{
	GSExecuteArgument *Owner = (GSExecuteArgument*) parameter;

	int ProcId = Owner->m_ExecuteType;
	int Id		= Owner->m_Id;

	while(TRUE)
	{
		if( WaitForSingleObject( Owner->m_ProcactorImpl->m_hKillEvent, 1 ) == WAIT_OBJECT_0 ){ break; }

		Owner->m_ProcactorImpl->Handle_Event(ProcId);
	}

	delete Owner;
	
	return 0;
}

bool GSProactorImpl::Handle_Event(int ProcId)
{
	if (m_JobMap.find(ProcId) == m_JobMap.end())
	{
		m_JobMap[ProcId] = new Concurrency::concurrent_queue<IMessagePtr>();
		m_ExecuteJobEvt[ProcId] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	IMessagePtr pJob;
	if(m_JobMap[ProcId]->try_pop(pJob) == FALSE)
	{
		Sleep(50);
		return false;
	}
		
	if(pJob != NULL)
		pJob->Execute(pJob->Message);
	else
		SYSLOG().Write("execute pjob is null");

//	if(m_JobList[ProcId].try_pop(pJob) == FALSE) 
//		ResetEvent(m_ExecuteJobEvt[ProcId]);

	return true;
}

}	}
