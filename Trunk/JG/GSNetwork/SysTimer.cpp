#include "stdafx.h"
#include "SysTimer.h"
#include "SysTimerJob.h"

#include "MSG_PLAYER_PACKET.h"
#include "MSG_PLAYER_TIME.h"
#include "GSServer.h"
#include "GSMainProcess.h"
#include "GSAllocator.h"
#include <boost/shared_ptr.hpp>
#include "GSClientMgr.h"


class GSAllocator;

SysTimer::SysTimer()
{
	Initialize();
}

SysTimer::~SysTimer()
{
}

void SysTimer::Start(LPVOID pServer)
{
	pGSServer = pServer;

	SysTimerJob *pJob = new SysTimerJob(); //m_SysTimerJobPool.malloc();

	pJob->Func = SysTimer::OnEvt;
	pJob->SetExpineTime(GetTickCount());
	pJob->SetId(SysTimer::SYS_TIMER);

	AddTimerJob(pJob);
}

void SysTimer::OnEvt(LPVOID Arg)
{
	SysTimerJob *pJob = (SysTimerJob *)Arg;

	switch(pJob->GetId())
	{
		case SysTimer::SYS_TIMER:
		{
			GSNetwork::MSG_PLAYER_TIME_PTR	    pPlayerTime = ALLOCATOR.Create<GSNetwork::MSG_PLAYER_TIME>();

			GSNetwork::GSServer::GSServer *Server = (GSNetwork::GSServer::GSServer *)GetSysTimer().pGSServer;
			
			pPlayerTime->pHandler= &Server->GetClientMgr();
			pPlayerTime->Type	 = MSG_TYPE_USER;
			pPlayerTime->SubType = ONTIME;
			pPlayerTime->pServer = Server;

			MAINPROC.RegisterCommand(pPlayerTime);
			
			SysTimerJob* pNewJob = new SysTimerJob();//GetSysTimer().m_SysTimerJobPool.malloc();

			pNewJob->Func = SysTimer::OnEvt;
			pNewJob->SetExpineTime(GetTickCount() + 1000);
			pNewJob->SetId(SysTimer::SYS_TIMER);

			GetSysTimer().AddTimerJob(pNewJob);
		}
		break;
	}

	if (pJob != NULL)
		//	GetSysTimer().m_SysTimerJobPool.destroy(pJob);
		delete pJob;
}

SysTimer &GetSysTimer()
{
	static SysTimer gTimer;
	return gTimer;
}
