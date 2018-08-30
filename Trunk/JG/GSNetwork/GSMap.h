#pragma once

#include <map>

template<typename  T>
class GSMap
{
public:
	GSMap(void)  { InitializeCriticalSection(&m_lock); }
	~GSMap(void) { DeleteCriticalSection(&m_lock);     }

	bool AddData(DWORD Key,T *Data)
	{
		EnterCriticalSection(&m_lock);
		for(int i=0;i<1000;i++)
		{
			if(m_Map.find(Key+i) == m_Map.end())
			{
				m_Map[Key+i] = Data;
				LeaveCriticalSection(&m_lock);
				return TRUE;
			}
		}
		LeaveCriticalSection(&m_lock);
		return FALSE;
	}


	T *GetData()
	{
		EnterCriticalSection(&m_lock);
		if(m_Map.size() == 0)
		{
			LeaveCriticalSection(&m_lock);
			return NULL;
		}

		std::map<DWORD,T*>::iterator iter = m_Map.begin();
		
		T *Data = iter->second;
		m_Map.erase(iter);

		LeaveCriticalSection(&m_lock);
		return Data;
	}

	T *GetData(DWORD Key)
	{
		EnterCriticalSection(&m_lock);
		if(m_Map.find(Key) == m_Map.end())
		{
			LeaveCriticalSection(&m_lock);
			return NULL;
		}

		std::map<DWORD,T*>::Iterator iter = m_Map[Key];
		T *Data = iter->second;
		m_Map.erase(iter);
		LeaveCriticalSection(&m_lock);
		return Data;
	}

	T *DeleteData(DWORD Key)
	{
		EnterCriticalSection(&m_lock);
		if(m_Map.find(Key) == m_Map.end())
		{
			LeaveCriticalSection(&m_lock);
			return NULL;
		}

		std::map<DWORD,T*>::Iterator iter = m_Map[Key];

		T *Data = iter->second;
		m_Map.erase(iter);

		LeaveCriticalSection(&m_lock);
		return Data;
	}

	DWORD Size() { return m_Map.size(); }

protected:

	std::map<DWORD,T*>		m_Map;

	CRITICAL_SECTION	m_lock;
};

