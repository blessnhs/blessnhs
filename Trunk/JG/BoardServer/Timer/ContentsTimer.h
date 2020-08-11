#pragma once

#include "GSTimerQ.h"

#include <boost/pool/object_pool.hpp>

#include "ContentsTimerJob.h"

class ContentsTimer : public GSFrames::GSTimerQ::GSTimerQ
{
public:
	ContentsTimer();
	~ContentsTimer();

	enum {
		SYS_TIMER  = 1,
	};

	static void CALLBACK OnEvt(LPVOID Arg);

	void Start();

	boost::object_pool<ContentsTimerJob>									m_TimerJobPool;
};

extern ContentsTimer &GetContentsTimer();