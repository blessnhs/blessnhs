#pragma once

#include "GSTimerQ.h"

#include <boost/pool/object_pool.hpp>

#include "SysTimerJob.h"

class SysTimer : public GSFrames::GSTimerQ::GSTimerQ  
{
public:
	SysTimer();
	~SysTimer();

	enum {
		SYS_TIMER  = 1,
	};

	static void CALLBACK OnEvt(LPVOID Arg);

	void Start(LPVOID pServer);

	boost::object_pool<SysTimerJob>									m_SysTimerJobPool;

	LPVOID pGSServer;
};

extern SysTimer &GetSysTimer();