#include "StdAfx.h"
#include "GSTimerQ.h"

namespace GSFrames	{	namespace GSTimerQ	{

GSTimerQ::GSTimerQ(void)
{
	SYSTEM_INFO SystemInfo; 
	GetSystemInfo(&SystemInfo);
	MaxThread = 256;//SystemInfo.dwNumberOfProcessors;
}

GSTimerQ::~GSTimerQ(void)
{
}

VOID GSTimerQ::SendMessage(LPVOID Arg)
{
	GSTimerQ *pTimer = (GSTimerQ *)Arg;

	CThreadSync Sync;

	std::map<DWORD,GSTimerJob*>::iterator iter = pTimer->m_TimeJobList.begin();
	GSTimerJob *pJob = NULL;
	while(iter != pTimer->m_TimeJobList.end())
	{
		pJob = (GSTimerJob *)iter->second;
		if(pJob == NULL) break;
		
		if(GetTickCount() >= pJob->GetExpineTime())
		{
			if(pJob->Func == NULL)
			{
				iter = pTimer->m_TimeJobList.erase(iter);
				break;
			}
			ULONG nFlags = WT_EXECUTEDEFAULT;
			WT_SET_MAX_THREADPOOL_THREADS(nFlags, pTimer->MaxThread);
			QueueUserWorkItem((LPTHREAD_START_ROUTINE)pJob->Func,(LPVOID)pJob,nFlags);

			iter = pTimer->m_TimeJobList.erase(iter);
		}
		else
			++iter;
	}
}

VOID GSTimerQ::Expine(PVOID Arg)
{
	GSTimerQ *pTimer = (GSTimerQ *)Arg;

	GSTimerQ::SendMessage((LPVOID)pTimer);
}


bool GSTimerQ::AddTimerJob(GSTimerJob *pJob)
{
	if(pJob == NULL) return false;

	CThreadSync Sync;

	int i;
	for(i=0;i<10000;i++)	
	{
		if(m_TimeJobList.end() == m_TimeJobList.find(pJob->GetExpineTime()+i))
		{
			m_TimeJobList[pJob->GetExpineTime()+i] = pJob;
			break;
		}
	}

	if(i >=  10000)
		SYSLOG().Write("erase evt = %d\n",i);

	return true;
}

bool GSTimerQ::DelTimerJob(GSTimerJob *pJob)
{
	if (pJob == NULL)
		return false;

	CThreadSync sync;

	iterator iter = m_TimeJobList.begin();

	while(iter != m_TimeJobList.end())
	{
		GSTimerJob *Job =  (GSTimerJob *)(iter->second);
		if(pJob == Job)
		{
			iter = m_TimeJobList.erase(iter);
		}
		else
			++iter;
	}
	
	return true;
}

int GSTimerQ::Initialize(int frequence)
{
	hTimer = NULL;
	hTimerQueue = NULL;
	int arg = 123;

	gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == gDoneEvent)
    {
        SYSLOG().Write("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }
	
	hTimerQueue = CreateTimerQueue();
    if (NULL == hTimerQueue)
    {
        SYSLOG().Write("CreateTimerQueue failed (%d)\n", GetLastError());
        return 2;
    }

    if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, 
            (WAITORTIMERCALLBACK)Expine, this , frequence, frequence, frequence))
    {
        SYSLOG().Write("CreateTimerQueueTimer failed (%d)\n", GetLastError());
        return 3;
    }
	return true;
}

}	}