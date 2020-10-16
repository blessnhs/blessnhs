template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::Add(CHANNEL_PTR &pObj)
{
	auto iter = m_ChannelMap.find(pObj->GetId());

	if(iter == m_ChannelMap.end())
	{
		m_ChannelMap[pObj->GetId()] = pObj;

		bool find = false;
		for (auto iter = m_ChannelMap.begin(); iter != m_ChannelMap.end(); iter++)
		{
			if (iter->second == NULL)
			{
				m_ChannelMap[iter->first] = pObj;
				return true;
			}
		}

		m_ChannelMap[pObj->GetId()] = pObj;
	}

	

	return false;
}

template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::Del(CHANNEL_PTR &pObj)
{
	auto iter = m_ChannelMap.find(pObj->GetId());

	if(iter == m_ChannelMap.end())
	{
		return false;
	}

	m_ChannelMap[pObj->GetId()] = NULL;


	bool find = false;
	for (auto iter = m_ChannelMap.begin(); iter != m_ChannelMap.end(); iter++)
	{
		if (iter->second != NULL)
		{
			if (iter->second->GetId() == pObj->GetId())
			{
				m_ChannelMap[iter->first] = NULL;
				return true;
			}

		}
	}


	return false;
}

template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::PlayerLeaveChannel(PLAYER_PTR player)
{
	auto iter = m_ChannelMap.find(player->GetChannel());

	if (iter == m_ChannelMap.end())
	{
		return false;
	}
	iter->second->PlayerContainer.Del2(player);

	return true;
}

template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::PlayerEnterChannel(PLAYER_PTR player)
{
	auto iter = m_ChannelMap.find(player->GetChannel());

	if (iter == m_ChannelMap.end())
	{
		auto channel = Create(player->GetChannel());
		channel->PlayerContainer.Add2(player);
		return true;
	}
	iter->second->PlayerContainer.Add2(player);

	return true;
}

template<template<class T> class CreationPolicy> CHANNEL_PTR ChannelContainer<CreationPolicy>::Search(INT64 Id)
{
	auto iter = m_ChannelMap.find(Id);

	if(iter == m_ChannelMap.end())
	{
		return CHANNEL_PTR();
	}
	return (iter->second);
}

template<template<class T> class CreationPolicy> CHANNEL_PTR ChannelContainer<CreationPolicy>::Create(INT64 Id)
{
	CHANNEL_PTR PTR =  CreationPolicy<CHANNEL>().Create();
	PTR->SetId(Id);

	m_ChannelMap[Id] = PTR;

	return PTR;
}


template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::RoomEnterChannel(INT64 Id, ROOM_PTR room)
{
	auto iter = m_ChannelMap.find(Id);

	if (iter == m_ChannelMap.end())
	{
		return false;
	}
	iter->second->RoomContainer.Add2(room);

	return true;
}

template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::RoomLeaveChannel(PLAYER_PTR player)
{
	auto iter = m_ChannelMap.find(player->GetChannel());

	if (iter == m_ChannelMap.end())
	{
		return false;
	}

	ROOM_PTR RoomPtr = iter->second->RoomContainer.Search(player->m_Char[0].GetRoom());
	if (RoomPtr != NULL)
	{
		iter->second->RoomContainer.Del2(RoomPtr);
		return true;
	}


	return false;
}

template<template<class T> class CreationPolicy> void ChannelContainer<CreationPolicy>::Report()
{
	for each(auto channel in m_ChannelMap)
	{
		printf("channel id %d\n", channel.second->GetId());
		printf("usermgr count  %d\n", channel.second->PlayerContainer.Count());
		printf("roommgr count  %d\n", channel.second->RoomContainer.Count());
	}
	
}

