#include "stdafx.h"
#include "ContentsTimer.h"
#include "ContentsTimerJob.h"


#include "GSAllocator.h"
#include <boost/shared_ptr.hpp>


class GSAllocator;

ContentsTimer::ContentsTimer()
{
	Initialize();
}

ContentsTimer::~ContentsTimer()
{
}

void ContentsTimer::Start()
{
	ContentsTimerJob *pJob = m_TimerJobPool.malloc();

	pJob->Func = ContentsTimer::OnEvt;
	pJob->SetExpineTime(GetTickCount());
	pJob->SetId(ContentsTimer::SYS_TIMER);

	AddTimerJob(pJob);
}

void ContentsTimer::OnEvt(LPVOID Arg)
{
	ContentsTimerJob *pJob = (ContentsTimerJob *)Arg;

	switch(pJob->GetId())
	{
		case ContentsTimer::SYS_TIMER:
		{
			PLAYERMGR.CheckUserList();

			ContentsTimerJob* pNewJob = GetContentsTimer().m_TimerJobPool.malloc();
			pNewJob->Func = ContentsTimer::OnEvt;
			pNewJob->SetExpineTime(GetTickCount() + 1000);
			pNewJob->SetId(ContentsTimer::SYS_TIMER);
			GetContentsTimer().AddTimerJob(pNewJob);
		}
		break;
	}

	if(pJob != NULL)
		GetContentsTimer().m_TimerJobPool.destroy(pJob);
}

ContentsTimer &GetContentsTimer()
{
	static ContentsTimer gTimer;
	return gTimer;
}
