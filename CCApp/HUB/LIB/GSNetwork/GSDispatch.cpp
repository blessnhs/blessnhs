#include "StdAfx.h"
#include "GSDispatch.h"

namespace GSFrames	{	namespace GSDispatch	{

DWORD WINAPI WorkerThread2(LPVOID parameter)
{
	GSDispatch *Owner = (GSDispatch*) parameter;
	Owner->WorkerThread2();

	return 0;
}

GSDispatch::GSDispatch(VOID)
{
	m_Handle			= NULL;
	m_WTCnt	= 0;
}

GSDispatch::~GSDispatch(VOID)
{
}

VOID GSDispatch::OnExecuteCallback(VOID *object)
{
	_DispatchJob *pJob = (_DispatchJob * )object;

	pJob->Job.Func((LPVOID)&pJob->Job);

	if(pJob != NULL)
		delete pJob;
}

VOID GSDispatch::Exe(_DispatchJob *pJob)
{
	PostQueuedCompletionStatus(m_Handle, 0, 0,pJob);
}

BOOL GSDispatch::Initialize(VOID)
{
	m_Handle			= NULL;

	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	m_WTCnt	= SystemInfo.dwNumberOfProcessors * 1;
	m_Handle			= CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (!m_Handle)
		return FALSE;

	for (DWORD i=0;i<m_WTCnt;i++)
	{
		HANDLE WorkerThread = CreateThread(NULL, 0, GSFrames::GSDispatch::WorkerThread2, this, 0, NULL);
		m_WTVector.push_back(WorkerThread);
	}

	return TRUE;
}

BOOL GSDispatch::Termination(VOID)
{
	for (DWORD i=0;i<m_WTVector.size();i++)
		PostQueuedCompletionStatus(m_Handle, 0, 0, NULL);

	for (DWORD i=0;i<m_WTVector.size();i++)
	{
		WaitForSingleObject(m_WTVector[i], INFINITE);

		CloseHandle(m_WTVector[i]);
	}

	if (m_Handle)
		CloseHandle(m_Handle);

	m_WTVector.clear();

	return TRUE;
}
VOID GSDispatch::WorkerThread2(VOID)
{
	BOOL			Successed				= FALSE;
	DWORD			NumberOfByteTransfered	= 0;
	VOID			*CompletionKey			= NULL;
	OVERLAPPED		*Overlapped				= NULL;
	_DispatchJob	*OverlappedEx			= NULL;
	VOID			*Object					= NULL;

	while (TRUE)
	{
		Successed = GetQueuedCompletionStatus(
			m_Handle,
			&NumberOfByteTransfered,
			(PULONG_PTR) &CompletionKey,
			&Overlapped,
			INFINITE);

		OverlappedEx	= (_DispatchJob*) Overlapped;

		OnExecuteCallback(OverlappedEx);
	}
}

}	}