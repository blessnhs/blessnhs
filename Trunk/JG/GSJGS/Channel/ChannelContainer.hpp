template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::Add(CHANNEL_PTR &pObj)
{
	std::map<DWORD,CHANNEL_PTR>::iterator iter;

	iter = m_ChannelMap.find(pObj->GetId());

	if(iter == m_ChannelMap.end())
	{
		m_ChannelMap[pObj->GetId()] = pObj;
		return true;;
	}

	return false;
}

template<template<class T> class CreationPolicy> bool ChannelContainer<CreationPolicy>::Del(CHANNEL_PTR &pObj)
{
	std::map<DWORD,CHANNEL_PTR>::iterator iter = m_ChannelMap.find(pObj->GetId());


	if(iter == m_ChannelMap.end())
	{
		return false;
	}

	m_ChannelMap.erase(iter);


	return true;
}


template<template<class T> class CreationPolicy> CHANNEL_PTR ChannelContainer<CreationPolicy>::Search(DWORD Id)
{
	std::map<DWORD,CHANNEL_PTR>::iterator iter = m_ChannelMap.find(Id);

	if(iter == m_ChannelMap.end())
	{
		return CHANNEL_PTR();
	}

	return (iter->second);
}


template<template<class T> class CreationPolicy> CHANNEL_PTR ChannelContainer<CreationPolicy>::Create()
{
	static DWORD Index = 1;

	CHANNEL_PTR PTR =  CreationPolicy<CHANNEL>().Create();

	PTR->SetId(Index++);

	return PTR;
}

template<template<class T> class CreationPolicy> void ChannelContainer<CreationPolicy>::Initialize()
{
	CHANNEL_PTR pPtr = Create();

	pPtr->SetId(0);

	Add(pPtr);
}

template<template<class T> class CreationPolicy> VOID ChannelContainer<CreationPolicy>::GetChannelList(std::vector<RoomInfo> &List)
{
	std::map<DWORD,CHANNEL_PTR>::iterator iter = m_ChannelMap.begin();
	RoomInfo info;

	if(iter != m_ChannelMap.end())
	{
		if(iter->second != NULL)
		{
			//memcpy(info.Name,iter->second->m_Stock.Name,sizeof(info.Name));
			info.Name = iter->second->m_Stock.Name;
			info.Index  = iter->second->GetId();
		}
		List.push_back(info);

		memset(&info,0,sizeof(info));
		
		iter++;
	}
}

