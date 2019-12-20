template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Add(ROOM_PTR &pObj)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,ROOM_PTR>::iterator iter;

	iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		m_RoomMap[pObj->GetId()] = pObj;
		LeaveCriticalSection(&m_PublicLock);
		return true;;
	}

	LeaveCriticalSection(&m_PublicLock);

	return false;
}

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Del(ROOM_PTR &pObj)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,ROOM_PTR>::iterator iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		LeaveCriticalSection(&m_PublicLock);
		return false;
	}

	m_RoomMap.erase(iter);
	LeaveCriticalSection(&m_PublicLock);

	return true;
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Search(DWORD Id)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,ROOM_PTR>::iterator iter = m_RoomMap.find(Id);

	if(iter == m_RoomMap.end())
	{
		LeaveCriticalSection(&m_PublicLock);
		return ROOM_PTR();
	}
	LeaveCriticalSection(&m_PublicLock);
	return (iter->second);
}

template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::SearchByGameType(WORD Type)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,ROOM_PTR>::iterator iter = m_RoomMap.begin();

	while(iter != m_RoomMap.end())
	{
		if(iter->second->m_Stock.GameType == Type)
		{
			if(iter->second->GetCurrPlayer() < iter->second->m_Stock.MAX_PLAYER)
			{
				LeaveCriticalSection(&m_PublicLock);
				return (iter->second) ;
			}
		}
		iter++;
	}
	LeaveCriticalSection(&m_PublicLock);
	return ROOM_PTR();
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Create()
{
	EnterCriticalSection(&m_PublicLock);

	static DWORD RoomIndex = 1;

	ROOM_PTR PTR =  CreationPolicy<ROOM>().Create();

	PTR->SetId(RoomIndex++);

	LeaveCriticalSection(&m_PublicLock);
	return PTR;
}


template<template<class T> class CreationPolicy> VOID RoomContainer<CreationPolicy>::GetRoomList(std::vector<RoomInfo> &List)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,ROOM_PTR>::iterator iter = m_RoomMap.begin();
	RoomInfo info;

	while(iter != m_RoomMap.end())
	{
		if(iter->second != NULL)
		{
			info.Name = iter->second->m_Stock.Name;
			info.Index  = iter->second->GetId();
		}
		List.push_back(info);

		//memset(&info,0,sizeof(info));
		info.Index = 0;
		
		iter++;
	}

	LeaveCriticalSection(&m_PublicLock);
}

