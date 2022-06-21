#pragma once

#include "Lock.h"

template <class T>
class GSGuard
{
friend class CThreadSync;
public:
	
	class CThreadSync
	{
	public:
		CThreadSync(VOID)
		{
			T::mSync.Enter();
		}

		~CThreadSync(VOID)
		{
			T::mSync.Leave();
		}
	};

private:
	static  CGSLock mSync;
};

template <class T>
 CGSLock GSGuard<T>::mSync;


