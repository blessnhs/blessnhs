template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Add(ROOM_PTR &pObj)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		m_RoomMap[pObj->GetId()] = pObj;
		return true;;
	}


	return false;
}

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Del(ROOM_PTR &pObj)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		return false;
	}

	m_RoomMap[pObj->GetId()] = NULL;

	return true;
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Search(DWORD Id)
{
	auto iter = m_RoomMap.find(Id);

	if(iter == m_RoomMap.end())
	{
		return ROOM_PTR();
	}
	return (iter->second);
}

template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::SearchByGameType(WORD Type)
{
	auto iter = m_RoomMap.begin();

	while(iter != m_RoomMap.end())
	{
		if(iter->second->m_Stock.GameType == Type)
		{
			if(iter->second->GetCurrPlayer() < iter->second->m_Stock.MAX_PLAYER)
			{
				return (iter->second) ;
			}
		}
		iter++;
	}
	return ROOM_PTR();
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Create()
{
	static atomic<int> intAtomic = 1;

	ROOM_PTR PTR =  CreationPolicy<ROOM>().Create();

	PTR->SetId(intAtomic);

	intAtomic.fetch_add(1);

	return PTR;
}


template<template<class T> class CreationPolicy> VOID RoomContainer<CreationPolicy>::GetRoomList(std::vector<RoomInfo> &List)
{
	auto iter = m_RoomMap.begin();
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
}

