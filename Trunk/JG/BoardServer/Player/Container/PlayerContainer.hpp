#include "../../Server/GSBoard.h"

class GSCLIENT;

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
	/*auto iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{*/
		m_PlayerMap[pObj->GetId()] = pObj;
		return true;;
	//}

	return false;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Del(PlayerPtr &pObj)
{

	auto iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		return false;
	}

	m_PlayerMap[pObj->GetId()] = NULL;

	return true;
}


template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Search(DWORD Id)
{
	auto iter = m_PlayerMap.find(Id);

	if(iter == m_PlayerMap.end())
	{
		return PlayerPtr();
	}

	return (iter->second);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::SearchBySocketId(DWORD Id)
{
	for each(auto& iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter->second;
		if (pPlayer == NULL)
			continue;

		if (pPlayer->GetPair() == Id)
		{
			return pPlayer;
		}
	}

	return PlayerPtr(0);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>:: Search(string flatformid)
{
	for each( auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (pPlayer->m_Account.GetFlatformId() == flatformid)
		{
			return pPlayer;
		}
	}
	
	return PlayerPtr(0);
}

template<template<class T> class CreationPolicy> int PlayerContainer<CreationPolicy>::Count()
{
	return m_PlayerMap.size();
}
template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Create()
{
	return CreationPolicy<Player>().Create();
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::CheckUserList()
{
	auto iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			string Name = iter->second->m_Account.GetName();
			WORD SessionId= iter->second->GetPair();

			GSCLIENT_PTR pSession = SERVER.GetClient(SessionId);
			if(!pSession)
				printf("Lost GSCLIENT %s %d \n",Name.c_str(),SessionId);
		}
		iter++;
	}
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
	auto iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			printf("%s\n",iter->second->m_Account.GetName().c_str());
		}
		++iter;
	}

	printf("\nasocket count %d\n",SERVER.GetClientMgr().GetActiveSocketCount());

	printf("Total Count %d\n",m_PlayerMap.size());
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length)
{
	auto iter = m_PlayerMap.begin();
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
}


template<template<class T> class CreationPolicy> template<class TYPE> void PlayerContainer<CreationPolicy>::BroadCast(TYPE MSG)
{
	auto iter = m_PlayerMap.begin();
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
}

template<template<class T> class CreationPolicy>  VOID PlayerContainer<CreationPolicy>::OnResponse(LPVOID Data)
{

}