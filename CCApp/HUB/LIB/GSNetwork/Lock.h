#pragma once

class CGSLock
{

public:
	CGSLock()
	{
		InitializeCriticalSection(&mSync);
	}

	~CGSLock()
	{
		DeleteCriticalSection(&mSync);
	}

	inline void Enter()
	{
		EnterCriticalSection(&mSync);
	}

	inline void Leave()
	{
		LeaveCriticalSection(&mSync);
	}

private:
	CRITICAL_SECTION	mSync;
};

