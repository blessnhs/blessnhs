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
	GSFrames::GSTimerJob::GSTimerJob*pJob = (GSFrames::GSTimerJob::GSTimerJob*)Arg;

	switch(pJob->GetId())
	{
		case ContentsTimer::SYS_TIMER:
		{
			PLAYERMGR.CheckUserList();

			ContentsTimerJob* pNewJob = new ContentsTimerJob();
			pNewJob->Func = ContentsTimer::OnEvt;
			pNewJob->SetExpineTime(GetTickCount() + 1000 * 60);
			pNewJob->SetId(ContentsTimer::SYS_TIMER);
			GetContentsTimer().AddTimerJob(pNewJob);
		}
		break;

		case ContentsTimer::RANK_TIMER:
		{
			{
				boost::shared_ptr<Hub::MSG_PLAYER_QUERY<Hub::CalcRank>>		PLAYER_MSG = ALLOCATOR.Create<Hub::MSG_PLAYER_QUERY<Hub::CalcRank>>();
				PLAYER_MSG->Type = MSG_TYPE_DB_1;
				PLAYER_MSG->SubType = ONQUERY;
				MAINPROC.RegisterCommand(PLAYER_MSG);
			}

			RankTimerJob* pNewJob = new RankTimerJob();
			pNewJob->Func = ContentsTimer::OnEvt;
			pNewJob->SetExpineTime(GetTickCount() + (1000 * 60 * 5));
			pNewJob->SetId(ContentsTimer::RANK_TIMER);
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
