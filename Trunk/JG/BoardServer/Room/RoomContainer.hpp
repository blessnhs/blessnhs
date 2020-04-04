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

template<template<class T> class CreationPolicy> VOID RoomContainer<CreationPolicy>::GetRoomList(google::protobuf::RepeatedPtrField<RoomInfo2>*List)
{
	//auto iter = m_RoomMap.begin();

	//while(iter != m_RoomMap.end())
	//{
	//	if(iter->second != NULL)
	//	{
	//		RoomInfo2 *info = List->Add();
	//		info->mutable_var_name()->assign( iter->second->m_Stock.Name );
	//		info->set_var_id( iter->second->GetId() );
	//		info->set_var_current_count(iter->second->GetCurrPlayer());
	//		info->set_var_max_count(-1);
	//	}
	//	
	//	iter++;
	//}
}
template<template<class T> class CreationPolicy> BOOL RoomContainer<CreationPolicy>::CreateMatchRoom(PLAYER_PTR target1, PLAYER_PTR target2)
{
	//�����
	/////////////////////////////////////////////////////////////////////////////
	GSCLIENT_PTR pSession1 = SERVER.GetClient(target1->GetPair());
	if (!pSession1)
		return FALSE;

	CREATE_ROOM_RES room_res;

	ROOM_PTR RoomPtr = Create();
	RoomPtr->m_Stock.Name.append(target1->m_Account.GetName());
	RoomPtr->m_Stock.Name.append("  VS  ");
	RoomPtr->m_Stock.Name.append(target2->m_Account.GetName());

	Add(RoomPtr);
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

	RoomPtr->SendNewUserInfo(target1);	//�濡 �ִ� �����鿡�� ���ο� ���� ��������


	//������
	/////////////////////////////////////////////////////////////////////////////{
	GSCLIENT_PTR pSession2 = SERVER.GetClient(target2->GetPair());
	if (!pSession2)
	{
		printf("Match Fail Not Found player 2\n");
		RoomPtr->RemovePlayer(target1);
		Del(RoomPtr);
		return FALSE;
	}

	ENTER_ROOM_RES enter_res;

	//�̹� ���� �� �ִٸ� 
	if (RoomPtr->FindPlayer(target2) == TRUE)
	{
		printf("Match Fail Exist player 2\n");
		RoomPtr->RemovePlayer(target1);
		Del(RoomPtr);
		return FALSE;
	}

	RoomPtr->InsertPlayer(target2);

	target2->m_Char[0].SetRoom( RoomPtr->GetId() );

	enter_res.set_var_room_id(RoomPtr->GetId());
	enter_res.set_var_name(RoomPtr->m_Stock.Name.c_str());
	SEND_PROTO_BUFFER(enter_res, pSession2)

	RoomPtr->SetState(Game);

	GetMatchMap()[target1->GetId()] = NULL;
	GetMatchMap()[target2->GetId()] = NULL;

	//���� ������ �������� ����� ���� ��������
	for each (auto iter in RoomPtr->m_PlayerMap)
	{
		if (iter.second == NULL)
			continue;

		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo * userinfo = nty.mutable_var_room_user();

		userinfo->set_var_index(iter.second->GetId());
		userinfo->set_var_name(iter.second->m_Account.GetName());

		SEND_PROTO_BUFFER(nty, pSession2)
	}

	//����� ������ ���� ���ο� ���� ������ ����
	for each (auto iter in RoomPtr->m_PlayerMap)
	{
		if (iter.second == NULL)
			continue;

		GSCLIENT_PTR pPair = SERVER.GetClient(iter.second->GetPair());
		if (pPair == NULL)
			continue;


		NEW_USER_IN_ROOM_NTY nty;

		RoomUserInfo * userinfo = nty.mutable_var_room_user();

		userinfo->set_var_index(iter.second->GetId());
		userinfo->set_var_name(target2->m_Account.GetName());

		SEND_PROTO_BUFFER(nty, pPair)
	}

	return TRUE;
	
}
