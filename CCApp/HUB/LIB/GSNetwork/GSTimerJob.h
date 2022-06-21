#pragma once

#include "GSObject.h"
#include "GSMemPool.h"


namespace GSFrames	{	namespace GSTimerJob	{

class GSTimerJob : public GSNetwork::GSObject::GSObject 
{
public:
	GSTimerJob(void);
	~GSTimerJob(void);

	VOID SetExpineTime(DWORD Time) { m_ExpineTime = Time; }
	DWORD GetExpineTime() { return m_ExpineTime; }

protected:
	DWORD m_ExpineTime;

public:
	void (__stdcall *Func)(LPVOID Arg);

};

}	}