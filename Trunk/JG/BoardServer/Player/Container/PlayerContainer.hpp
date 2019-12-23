template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::PlayerContainer()
{
	InitializeCriticalSection(&m_PublicLock);
}
template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::~PlayerContainer()
{
	DeleteCriticalSection(&m_PublicLock);
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Add(PlayerPtr &pObj)
{
	EnterCriticalSection(&m_PublicLock);

	auto iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		m_PlayerMap[pObj->GetId()] = pObj;
		LeaveCriticalSection(&m_PublicLock);
		return true;;
	}

	LeaveCriticalSection(&m_PublicLock);
	return false;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Del(PlayerPtr &pObj)
{
	EnterCriticalSection(&m_PublicLock);

	auto iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		LeaveCriticalSection(&m_PublicLock);
		return false;
	}

	LeaveCriticalSection(&m_PublicLock);
	m_PlayerMap.erase(iter);

	return true;
}


template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Search(DWORD Id)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.find(Id);

	if(iter == m_PlayerMap.end())
	{
		LeaveCriticalSection(&m_PublicLock);
		return PlayerPtr();
	}

	LeaveCriticalSection(&m_PublicLock);
	return (iter->second);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::SearchBySocketId(DWORD Id)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD, PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while (iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if (pPlayer->GetPair() == Id)
		{
			LeaveCriticalSection(&m_PublicLock);
			return pPlayer;
		}

		++iter;
	}
	LeaveCriticalSection(&m_PublicLock);
	return PlayerPtr(0);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>:: Search(wstring Account)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer->m_Account.GetName() == Account)
		{
			LeaveCriticalSection(&m_PublicLock);
			return pPlayer;
		}

		++iter;
	}
	LeaveCriticalSection(&m_PublicLock);
	return PlayerPtr(0);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Create()
{
	return CreationPolicy<Player>().Create();
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::GetUserList(std::vector<UserInfo> &List)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	UserInfo info;

	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			info.Name = iter->second->m_Account.GetName();
		}
		List.push_back(info);

		memset(&info,0,sizeof(info));

		iter++;
	}

	LeaveCriticalSection(&m_PublicLock);
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::CheckUserList()
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			wstring Name = iter->second->m_Account.GetName();
			WORD SessionId= iter->second->GetPair();

			GSCLIENT *pSession = JGSERVER.GetClient(SessionId);
			if(!pSession)
				printf("Lost GSCLIENT %S %d \n",Name.c_str(),SessionId);
		}
		iter++;
	}
	LeaveCriticalSection(&m_PublicLock);
}


/*
template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::BroadCast(LPVOID MSG)
{
	
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			GSCLIENT *pPair =IOCP.GetSessionMgr()->GetSession(iter->second->GetPair());

			if(pPair != NULL)
			{
				Sender::Send(pPair,MSG);
			}
		}
		++iter;
	}
	
}*/

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::Print()
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			printf("%S\n",iter->second->m_Account.GetName().c_str());
		}
		++iter;
	}

	LeaveCriticalSection(&m_PublicLock);

	printf("\nasocket count %d\n",JGSERVER.GetClientMgr().GetActiveSocketCount());

	printf("Total Count %d\n",m_PlayerMap.size());
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			GSCLIENT *pPair =JGSERVER.GetClient(iter->second->GetPair());
			if(pPair != NULL)
			{
				pPair->GetTCPSocket()->WritePacket(MainId,SudbId,Message,Length);
			}
		}
		++iter;
	}

	LeaveCriticalSection(&m_PublicLock);
}


template<template<class T> class CreationPolicy> template<class TYPE> void PlayerContainer<CreationPolicy>::BroadCast(TYPE MSG)
{
	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			GSCLIENT *pPair =IOCP.GetSessionMgr()->GetSession(iter->second->GetPair());
			if(pPair != NULL)
			{
				Sender::Send(pPair,MSG);
			}
		}
		++iter;
	}

	LeaveCriticalSection(&m_PublicLock);
}

template<template<class T> class CreationPolicy>  VOID PlayerContainer<CreationPolicy>::OnResponse(LPVOID Data)
{

}