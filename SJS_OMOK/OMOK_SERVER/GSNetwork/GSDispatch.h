#pragma once

#include "GSTimerJob.h"

namespace GSFrames	{	namespace GSDispatch	{

typedef GSFrames::GSTimerJob::GSTimerJob	GSTimerJob;

struct _DispatchJob : public OVERLAPPED
{
	GSTimerJob  Job;
} ;

class GSDispatch : public GSGuard<GSDispatch>
{
public:
	GSDispatch(void);
	~GSDispatch(void);

	HANDLE				m_Handle;
	DWORD				m_WTCnt;

	std::vector<HANDLE>	m_WTVector;

private:
	VOID		OnExecuteCallback(VOID *object);

public:
	VOID		Exe(_DispatchJob *pJob);

public:
	BOOL		Initialize(VOID);
	BOOL		Termination(VOID);
	
	VOID		WorkerThread2(VOID);
};

}	}