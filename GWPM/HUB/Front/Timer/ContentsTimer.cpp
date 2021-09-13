#include "stdafx.h"
#include "ContentsTimer.h"
#include "ContentsTimerJob.h"
#include "RankTimerJob.h"



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
	{
		ContentsTimerJob* pJob = new ContentsTimerJob();

		pJob->Func = ContentsTimer::OnEvt;
		pJob->SetExpineTime(GetTickCount());
		pJob->SetId(ContentsTimer::SYS_TIMER);

		AddTimerJob(pJob);
	}
	/////////////////////////////////////////////////
	{
		RankTimerJob* pJob = new RankTimerJob();

		pJob->Func = ContentsTimer::OnEvt;
		pJob->SetExpineTime(GetTickCount());
		pJob->SetId(ContentsTimer::RANK_TIMER);

		AddTimerJob(pJob);
	}
}

void ContentsTimer::OnEvt(LPVOID Arg)
{
	ContentsTimerJob *pJob = (ContentsTimerJob *)Arg;

	switch(pJob->GetId())
	{
		case ContentsTimer::SYS_TIMER:
		{
			PROXYHUB.TryConnect();

			ContentsTimerJob* pNewJob = new ContentsTimerJob();
			pNewJob->Func = ContentsTimer::OnEvt;
			pNewJob->SetExpineTime(GetTickCount() + 1000);
			pNewJob->SetId(ContentsTimer::SYS_TIMER);
			GetContentsTimer().AddTimerJob(pNewJob);
		}
		break;
	}

	if (pJob != NULL)
		delete pJob;
}

ContentsTimer &GetContentsTimer()
{
	static ContentsTimer gTimer;
	return gTimer;
}
