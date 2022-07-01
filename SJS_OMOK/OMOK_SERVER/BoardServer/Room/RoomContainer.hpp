#include "../Channel/ChannelContainer.h"

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::Add(ROOM_PTR &pObj, PLAYER_PTR player)
{
	auto iter = m_RoomMap.find(pObj->GetId());

	if(iter == m_RoomMap.end())
	{
		m_RoomMap[pObj->GetId()] = pObj;

		//룸 매니저는 통합 매니저 없애고 채널에 귀속
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

template<template<class T> class CreationPolicy> concurrency::concurrent_unordered_map<INT64, PLAYER_PTR>& RoomContainer<CreationPolicy>::GetMatchMap()
{
	return m_MatchMap;
}

template<template<class T> class CreationPolicy> void RoomContainer<CreationPolicy>::AddMatchMap(PLAYER_PTR player)
{
	for each (auto iter in m_MatchMap)
	{
		if (iter.second == NULL)
		{
			iter.second = player;
		}
	}

	m_MatchMap[player->GetId()] = player;
}

template<template<class T> class CreationPolicy> bool RoomContainer<CreationPolicy>::IsExistMatchMap(PLAYER_PTR player)
{
	for each (auto iter in m_MatchMap)
	{
		if (iter.second == NULL)
		{
			continue;
		}

		if (iter.second->GetId() == player->GetId())
		{
			return true;
		}
	}

	return false;
}

template<template<class T> class CreationPolicy> void RoomContainer<CreationPolicy>::DelMatchMap(PLAYER_PTR player)
{
	for each (auto iter in m_MatchMap)
	{
		if (iter.second != NULL)
		{
			if (iter.second->GetId() == player->GetId())
			{
				m_MatchMap[player->GetId()] = NULL;
			}
		}
	}
}


template<template<class T> class CreationPolicy> void RoomContainer<CreationPolicy>::LeaveRoomPlayer(PLAYER_PTR pPlayer)
{
	LEAVE_ROOM_RES res;

	ROOM_PTR RoomPtr = Search(pPlayer->m_Char[0].GetRoom());
	if (RoomPtr != NULL)
	{
		if (RoomPtr->FindPlayer(pPlayer) != USHRT_MAX)
		{
			//승패 처리 간으한지의 여부 방안에 2명이 존재 했고 방이종료되지 않았으면 승패를 내본다.
			if (RoomPtr->GetState() != State::End && RoomPtr->GetCurrPlayer() > 1)
			{
				RoomPtr->SetState(State::End);

				auto OppPlayer = RoomPtr->GetOtherPlayer(pPlayer->GetId());
				if (OppPlayer != NULL)
				{
					RoomPtr->RecoardResult(OppPlayer, pPlayer);

				}
			}

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


template<template<class T> class CreationPolicy> BOOL RoomContainer<CreationPolicy>::CreateMatchRoom(PLAYER_PTR target1, PLAYER_PTR target2)
{
	//방생성
	/////////////////////////////////////////////////////////////////////////////
	GSCLIENT_PTR pSession1 = SERVER.GetClient(target1->GetPair());
	if (!pSession1)
		return FALSE;

	CREATE_ROOM_RES room_res;

	ROOM_PTR RoomPtr = Create();
	RoomPtr->m_Stock.Name.append(target1->m_Account.GetName());
	RoomPtr->m_Stock.Name.append(" ");
	string lvstring = RoomPtr->LevelConverter(target1->m_Char[0].GetLevel());

	RoomPtr->m_Stock.Name.append(lvstring);

	RoomPtr->m_Stock.Name.append("  VS  ");

	RoomPtr->m_Stock.Name.append(target2->m_Account.GetName());
	RoomPtr->m_Stock.Name.append(" ");

	string lvstring2 = RoomPtr->LevelConverter(target2->m_Char[0].GetLevel());
	RoomPtr->m_Stock.Name.append(lvstring2);

	Add(RoomPtr, target1);
	RoomPtr->m_Stock.MAX_PLAYER = USHRT_MAX;

	if (target1 != NULL)
	{
		target1->m_Char[0].SetRoom( RoomPtr->GetId() );
	}

	RoomPtr->InsertPlayer(target1);
	RoomPtr->SetState(Prepare);

	room_res.set_var_room_id(RoomPtr->GetId());
	room_res.set_var_name(RoomPtr->m_Stock.Name);
	SEND_PROTO_BUFFER(room_res, pSession1)


	//방입장
	/////////////////////////////////////////////////////////////////////////////{
	GSCLIENT_PTR pSession2 = SERVER.GetClient(target2->GetPair());
	if (!pSession2)
	{
		printf("Match Fail Not Found player 2\n");
		RoomPtr->RemovePlayer(target1);
		Del(RoomPtr, target1);
		return FALSE;
	}

	ENTER_ROOM_RES enter_res;

	//이미 입장 해 있다면 
	if (RoomPtr->FindPlayer(target2) == TRUE)
	{
		printf("Match Fail Exist player 2\n");
		RoomPtr->RemovePlayer(target1);
		Del(RoomPtr, target1);
		return FALSE;
	}

	RoomPtr->InsertPlayer(target2);

	target2->m_Char[0].SetRoom( RoomPtr->GetId() );

	enter_res.set_var_room_id(RoomPtr->GetId());
	enter_res.set_var_name(RoomPtr->m_Stock.Name.c_str());
	SEND_PROTO_BUFFER(enter_res, pSession2)

	RoomPtr->SetState(Game);

	//이제 매칭 맵에서 제거한다.
	DelMatchMap(target1);
	DelMatchMap(target2);



	//1p에게는 2p유저 정보 전송
	{
		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo * userinfo = nty.mutable_var_room_user();

		userinfo->set_var_index(target2->GetId());
		userinfo->set_var_name(target2->m_Account.GetName());
		userinfo->set_picture_uri(target2->m_Account.GetPicture_url());

		SEND_PROTO_BUFFER(nty, pSession1)
	}

	//2p에게는 1p유저 정보 전송
	{

		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo * userinfo = nty.mutable_var_room_user();

		userinfo->set_var_index(target1->GetId());
		userinfo->set_var_name(target1->m_Account.GetName());
		userinfo->set_picture_uri(target1->m_Account.GetPicture_url());

		SEND_PROTO_BUFFER(nty, pSession2)
	}

	return TRUE;
	
}
