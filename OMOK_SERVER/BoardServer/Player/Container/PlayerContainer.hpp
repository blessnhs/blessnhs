#include "../../Server/GSBoard.h"
#include "../../Room/RoomContainer.h"

class GSCLIENT;

template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::PlayerContainer()
{
	InitializeCriticalSection(&m_PublicLock);
}
template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::~PlayerContainer()
{
	DeleteCriticalSection(&m_PublicLock);
}


template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Add2(PlayerPtr& pObj)
{
	m_PlayerMap[pObj->GetId()] = pObj;

	bool find = false;
	for (auto iter = m_PlayerMapForLoop.begin(); iter != m_PlayerMapForLoop.end(); iter++)
	{
		if (iter->second == NULL)
		{
			m_PlayerMapForLoop[iter->first] = pObj;
			return true;
		}
	}

	m_PlayerMapForLoop[pObj->GetId()] = pObj;

	return true;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Del2(PlayerPtr& pObj)
{
	auto iter = m_PlayerMap.find(pObj->GetId());

	if (iter == m_PlayerMap.end())
	{
		return false;
	}

	m_PlayerMap[pObj->GetId()] = NULL;

	///////////////////////////////////////////////////

	bool find = false;
	for (auto iter = m_PlayerMapForLoop.begin(); iter != m_PlayerMapForLoop.end(); iter++)
	{
		if (iter->second != NULL)
		{
			if (iter->second->GetId() == pObj->GetId())
			{
				m_PlayerMapForLoop[iter->first] = NULL;
				return true;
			}

		}
	}


	return false;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Add(PlayerPtr &pObj)
{
	m_PlayerMap[pObj->GetId()] = pObj;

	CHANNELMGR.PlayerEnterChannel( pObj);

	bool find = false;
	for (auto iter = m_PlayerMapForLoop.begin(); iter != m_PlayerMapForLoop.end(); iter++)
	{
		if (iter->second == NULL)
		{
			m_PlayerMapForLoop[iter->first] = pObj;
			return true;
		}
	}

	m_PlayerMapForLoop[pObj->GetId()] = pObj;

	return true;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Del(PlayerPtr &pObj)
{

	auto iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		return false;
	}

	CHANNELMGR.PlayerLeaveChannel(pObj);


	m_PlayerMap[pObj->GetId()] = NULL;

	///////////////////////////////////////////////////

	bool find = false;
	for (auto iter = m_PlayerMapForLoop.begin(); iter != m_PlayerMapForLoop.end(); iter++)
	{
		if (iter->second != NULL)
		{
			if (iter->second->GetId() == pObj->GetId())
			{
				m_PlayerMapForLoop[iter->first] = NULL;
				return true;
			}
			
		}
	}


	return false;
}


template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Search(INT64 Id)
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
	int count = 0;
	auto iter = m_PlayerMapForLoop.begin();

	while (iter != m_PlayerMapForLoop.end())
	{
		if (iter->second != NULL)
		{
			count++;
		}
		iter++;
	}

	return count;
}
template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Create()
{
	return CreationPolicy<Player>().Create();
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::CheckUserList()
{
	for (auto iter = m_PlayerMapForLoop.begin(); iter != m_PlayerMapForLoop.end(); iter++)
	{
		if (iter->second == NULL)
			continue;

		WORD SessionId = iter->second->GetPair();

		//미아가 된 player들
		GSCLIENT_PTR pSession = SERVER.GetClient(SessionId);
		if (pSession == NULL)
		{
			PlayerPtr pPlayer = iter->second;
			if (pPlayer != NULL)
			{
				printf("Client session is null mia\n");

				auto channel = CHANNELMGR.Search(pPlayer->GetChannel());
				if (channel == NULL)
				{
					continue;
				}

				channel->RoomContainer.LeaveRoomPlayer(pPlayer);

				pPlayer->SetPair(ULONG_MAX);
				PLAYERMGR.Del(pPlayer);

				//매칭 큐에서 제거한다.
				channel->RoomContainer.DelMatchMap(pPlayer);

				//로그아웃 쿼리를 날린다.

				boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestLogout>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestLogout>>();
				PLAYER_MSG->pRequst.Index = pPlayer->GetId();
				PLAYER_MSG->Type = MSG_TYPE_DB_1;				//그냥 디폴트
				PLAYER_MSG->SubType = ONQUERY;
				MAINPROC.RegisterCommand(PLAYER_MSG);
			}
			
		}
		
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
	auto iter = m_PlayerMapForLoop.begin();
	while(iter != m_PlayerMapForLoop.end())
	{
		if(iter->second != NULL)
		{
			printf("%s\n",iter->second->m_Account.GetName().c_str());
		}
		++iter;
	}

	printf("\nasocket count %d\n",SERVER.GetClientMgr().GetActiveSocketCount());

	printf("Total Count %d\n", m_PlayerMapForLoop.size());
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length)
{
	auto iter = m_PlayerMapForLoop.begin();
	while(iter != m_PlayerMapForLoop.end())
	{
		if(iter->second != NULL)
		{
			GSCLIENT *pPair = SERVER.GetClient(iter->second->GetPair());
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
	auto iter = m_PlayerMapForLoop.begin();
	while(iter != m_PlayerMapForLoop.end())
	{
		if(iter->second != NULL)
		{
			GSCLIENT* pPair = SERVER.GetClient(iter->second->GetPair());
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