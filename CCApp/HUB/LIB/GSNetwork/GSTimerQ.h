#pragma once

#include "GSTimerJob.h"


template <class T>
class GSGuard;

namespace GSFrames	{	namespace GSTimerQ	{

typedef GSFrames::GSTimerJob::GSTimerJob	GSTimerJob;

class GSTimerQ : public GSGuard<GSTimerQ>
{
public:
	typedef std::map<DWORD,GSTimerJob*>::iterator iterator;

public:
	GSTimerQ(void);
	virtual ~GSTimerQ(void);

	int Initialize(int frequence=1000);

	bool AddTimerJob(GSTimerJob *pJob);
	bool DelTimerJob(GSTimerJob *pJob);

	static VOID CALLBACK SendMessage(LPVOID arg);

	int GetSize() { return (int)m_TimeJobList.size(); }

	static VOID CALLBACK Expine(PVOID lpParam);

protected:
	
	HANDLE hTimer;
	HANDLE hTimerQueue;
	HANDLE gDoneEvent;

public:
	int    MaxThread;
	CRITICAL_SECTION cs;
protected:
//	tbb::concurrent_queue<GSTimerJob*> m_TimeJobList;
	std::map<DWORD,GSTimerJob*> m_TimeJobList;

	
};

}	}