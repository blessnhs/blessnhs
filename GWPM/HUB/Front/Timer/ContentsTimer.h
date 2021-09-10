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
		RANK_TIMER ,
	};

	static void CALLBACK OnEvt(LPVOID Arg);

	void Start();

};

extern ContentsTimer &GetContentsTimer();