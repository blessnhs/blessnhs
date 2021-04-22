
template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Add(ROOM_PTR &pObj, PLAYER_PTR player)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		m_RoomMap[pObj->GetId()] = pObj;

		//�� �Ŵ����� ���� �Ŵ��� ���ְ� ä�ο� �ͼ�
	/*	if (player != NULL)
			CHANNELMGR.RoomEnterChannel(player->GetChannel(), pObj);
		else
			printf("cant enter channel %d\n", player->GetChannel());*/

		bool find = false;
		for (auto iter = m_RoomMapForLoop.begin(); iter != m_RoomMapForLoop.end(); iter++)
		{
			if (iter->second == NULL)
			{
				m_RoomMapForLoop[iter->first] = pObj;
				return true;
			}
		}

		m_RoomMapForLoop[pObj->GetId()] = pObj;
	}

	

	return false;
}

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Del(ROOM_PTR &pObj, PLAYER_PTR player)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		return false;
	}

	m_RoomMap[pObj->GetId()] = NULL;

	//if (player != NULL)
	//	CHANNELMGR.RoomLeaveChannel(player);
	//else
	//	printf("cant leave channel %d\n", player->GetChannel());

	bool find = false;
	for (auto iter = m_RoomMapForLoop.begin(); iter != m_RoomMapForLoop.end(); iter++)
	{
		if (iter->second != NULL)
		{
			if (iter->second->GetId() == pObj->GetId())
			{
				m_RoomMapForLoop[iter->first] = NULL;


				return true;
			}

		}
	}


	return false;
}


template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Add2(ROOM_PTR& pObj)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if (iter == m_RoomMap.end())
	{
		m_RoomMap[pObj->GetId()] = pObj;

		bool find = false;
		for (auto iter = m_RoomMapForLoop.begin(); iter != m_RoomMapForLoop.end(); iter++)
		{
			if (iter->second == NULL)
			{
				m_RoomMapForLoop[iter->first] = pObj;
				return true;
			}
		}

		m_RoomMapForLoop[pObj->GetId()] = pObj;
	}



	return false;
}

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Del2(ROOM_PTR& pObj)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if (iter == m_RoomMap.end())
	{
		return false;
	}

	m_RoomMap[pObj->GetId()] = NULL;

	bool find = false;
	for (auto iter = m_RoomMapForLoop.begin(); iter != m_RoomMapForLoop.end(); iter++)
	{
		if (iter->second != NULL)
		{
			if (iter->second->GetId() == pObj->GetId())
			{
				m_RoomMapForLoop[iter->first] = NULL;

				return true;
			}

		}
	}

	return false;
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Search(INT64 Id)
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
	for each (auto room in m_RoomMap)
	{
		if (room->second->m_Stock.GameType == Type)
		{
			if (room->second->GetCurrPlayer() < room->second->m_Stock.MAX_PLAYER)
			{
				return (room->second);
			}
		}
	}

	return ROOM_PTR();
}


template<template<class T> class CreationPolicy> ROOM_PTR RoomContainer<CreationPolicy>::Create()
{
	EnterCriticalSection(&m_PublicLock);

	static atomic<int> intAtomic = 1;

	ROOM_PTR PTR =  CreationPolicy<ROOM>().Create();

	PTR->SetId(intAtomic);

	intAtomic.fetch_add(1);

	LeaveCriticalSection(&m_PublicLock);

	return PTR;
}

template<template<class T> class CreationPolicy> VOID RoomContainer<CreationPolicy>::GetRoomList(google::protobuf::RepeatedPtrField<RoomInfo2>*List)
{
	const int max_count = 10;
	int currcount = 0;

	for each (auto room in m_RoomMapForLoop)
	{
		if (room.second != NULL)
		{
			if (currcount > max_count)
				break;

			RoomInfo2* info = List->Add();
			info->mutable_var_name()->assign(room.second->m_Stock.Name);
			info->set_var_id(room.second->GetId());
			info->set_var_current_count(room.second->GetCurrPlayer());
			info->set_var_max_count(-1);

			currcount++;
		}
	}
}

template<template<class T> class CreationPolicy> int RoomContainer<CreationPolicy>::Count()
{
	int count = 0;
	for each (auto room in m_RoomMapForLoop)
	{
		if (room.second != NULL)
			count++;
	}

	return count;
}

template<template<class T> class CreationPolicy>  const concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>& RoomContainer<CreationPolicy>::GetRoomMap()
{
	return m_RoomMap;
}
template<template<class T> class CreationPolicy> void RoomContainer<CreationPolicy>::LeaveRoomPlayer(PLAYER_PTR pPlayer)
{
	LEAVE_ROOM_RES res;

	ROOM_PTR RoomPtr = Search(pPlayer->m_Char[0].GetRoom());
	if (RoomPtr != NULL)
	{
		if (RoomPtr->FindPlayer(pPlayer) != USHRT_MAX)
		{

			res.set_var_index(pPlayer->GetId());
			res.set_var_code(Success);
			res.mutable_var_name()->assign(pPlayer->m_Account.GetName());

			RoomPtr->SendToAll(res);

			RoomPtr->RemovePlayer(pPlayer);
			pPlayer->m_Char[0].SetRoom(0);

			if (RoomPtr->GetCurrPlayer() == 0)
				Del(RoomPtr, pPlayer);
		}

		pPlayer->m_Char[0].SetAllComplete(FALSE);
		pPlayer->m_Char[0].SetReady(FALSE);


	}
}
