#include "StdAfx.h"
#include "GSReactorImpl.h"
#include "GSExecuteArgument.h"
#include <process.h>

namespace GSFrames	{	namespace GSReactorImpl	{

unsigned int __stdcall HandleEvent(LPVOID parameter)
{
	GSReactorImpl *Owner = (GSReactorImpl*) parameter;

	Owner->_HandleEvent();

	return 0;
}

void GSReactorImpl::_HandleEvent()
{
	while(TRUE)
	{
		if( WaitForSingleObject( m_hKillEvent, 10 ) == WAIT_OBJECT_0 ){ break; }

		if(m_JobList.unsafe_size() == 0) continue;
	
		IMessagePtr pJob = GetMsg();

		if(pJob == NULL) continue;

		pJob->pHandler->OnEvt(pJob);

		//delete pJob;
	}
}

GSReactorImpl::GSReactorImpl()
{
}


GSReactorImpl::~GSReactorImpl(void)
{
}

int GSReactorImpl::Initialize()
{
	m_hKillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == m_hKillEvent)
    {
        SYSLOG().Write("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }
	unsigned int WorkerId = 0;
	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, HandleEvent, (LPVOID)this, 0, &WorkerId));

	return 0;
}

void GSReactorImpl::Terminate()
{
	SetEvent(m_hKillEvent);
}


bool GSReactorImpl::Register(IMessagePtr Msg)
{
	m_JobList.push(Msg);

	return TRUE;
}

bool GSReactorImpl::Remove(IMessagePtr Msg)
{
	return TRUE;
}

IMessagePtr GSReactorImpl::GetMsg()
{
	if(m_JobList.unsafe_size() == 0) return IMessagePtr();

	IMessagePtr pHandler;
	m_JobList.try_pop(pHandler);
	return pHandler;
}
}	}
