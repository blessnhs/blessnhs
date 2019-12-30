#include "StdAfx.h"
#include "GSProactorImpl.h"
#include "GSExecuteArgument.h"

namespace GSFrames	{	namespace GSProactorImpl	{

GSProactorImpl::GSProactorImpl(void)
{
	InitializeCriticalSectionAndSpinCount(&m_Lock,100);
}


GSProactorImpl::~GSProactorImpl(void)
{
}

bool GSProactorImpl::Create(int InputTheadCnt)
{
	m_hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_InputJobEvt	= CreateEvent(NULL, TRUE, FALSE, NULL);

	for(int i=0;i< MAX_QUUEUE_SIZE;i++)
		m_ExecuteJobEvt[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (NULL == m_hKillEvent || NULL == m_InputJobEvt || NULL == m_ExecuteJobEvt)
	{
		printf("Create Kill Event failed (%d)\n", GetLastError());
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
	m_JobList[Type].push(Msg);
	SetEvent(m_ExecuteJobEvt[Type]);
	
	return TRUE;
}

unsigned int __stdcall DistributionThread(LPVOID parameter)
{
	GSProactorImpl *Owner = (GSProactorImpl*) parameter;

	while(TRUE)
	{
		if( WaitForSingleObject( Owner->m_hKillEvent, 10 ) == WAIT_OBJECT_0 ){ break; }

//		WaitForSingleObject( Owner->m_InputJobEvt, 10 ) ;
		
		IMessagePtr pJob;
		if(Owner->m_InputJobList.try_pop(pJob) == FALSE) 
		{
			continue;
		}
	
		if(pJob != NULL)
		{
			Owner->Register(pJob->Type,pJob);
		}
		else
		{
			printf("pJob is null");
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
		if( WaitForSingleObject( Owner->m_ProcactorImpl->m_hKillEvent, 10 ) == WAIT_OBJECT_0 ){ break; }

//		WaitForSingleObject( Owner->m_ProcactorImpl->m_ExecuteJobEvt[ProcId], 10 ) ;

		Owner->m_ProcactorImpl->Handle_Event(ProcId);

		Sleep(0);
	}
	
	return 0;
}

bool GSProactorImpl::Handle_Event(int ProcId)
{
	IMessagePtr pJob;
	if(m_JobList[ProcId].try_pop(pJob) == FALSE) 
	{
		return false;
	}
		
	if(pJob != NULL)
		pJob->Execute(pJob->Message);
	else
		printf("execute pjob is null");

//	if(m_JobList[ProcId].try_pop(pJob) == FALSE) 
//		ResetEvent(m_ExecuteJobEvt[ProcId]);

	return true;
}

}	}
