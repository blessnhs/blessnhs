#pragma once
#include<queue>

template<typename  T>
class GSQueue 
{
public:
	GSQueue(void)
	{
		InitializeCriticalSection(&m_lock);
	}
	~GSQueue(void)
	{
		DeleteCriticalSection(&m_lock);
	}

public:

	void Push(T *Data)
	{
		EnterCriticalSection(&m_lock);
		m_Queue.push(Data);
		LeaveCriticalSection(&m_lock);
	}
	T *Pop()
	{
		EnterCriticalSection(&m_lock);

		if(m_Queue.size() == 0)
		{
			LeaveCriticalSection(&m_lock);
			return NULL;
		}
		T *Data = m_Queue.front();
		m_Queue.pop();
		LeaveCriticalSection(&m_lock);
		return Data;
	}
	DWORD Size()
	{
		//EnterCriticalSection(&m_lock);
		DWORD Cnt = m_Queue.size();
		//LeaveCriticalSection(&m_lock);
		return Cnt;
	}

private:
	std::queue<T*>		m_Queue;
	CRITICAL_SECTION	m_lock;
};
