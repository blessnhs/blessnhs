template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::PlayerContainer()
{

}
template<template<class T> class CreationPolicy> PlayerContainer<CreationPolicy>::~PlayerContainer()
{

}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Add(PlayerPtr &pObj)
{
	DWORD MaxUsr = INI.m_MaxUser;

	std::map<DWORD,PlayerPtr>::iterator iter;

	iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		m_PlayerMap[pObj->GetId()] = pObj;
		return true;;
	}

	return false;
}

template<template<class T> class CreationPolicy> bool PlayerContainer<CreationPolicy>::Del(PlayerPtr &pObj)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.find(pObj->GetId());


	if(iter == m_PlayerMap.end())
	{
		return false;
	}

	m_PlayerMap.erase(iter);


	return true;
}


template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Search(DWORD Id)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.find(Id);

	if(iter == m_PlayerMap.end())
	{
		return PlayerPtr();
	}

	return (iter->second);
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>:: Search(wstring Account)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer->m_Account.GetName() == Account)
		{
			return pPlayer;
		}

		++iter;
	}

	return PlayerPtr(0);
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::DeleteAllByServerId(WORD ServerId)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;

		if(pPlayer == NULL)
		{
			++iter;
			continue;;
		}

		if(pPlayer->m_GameServerId == ServerId)
		{
			iter = m_PlayerMap.erase(iter);
			continue;
		}

		++iter;
	}
}

template<template<class T> class CreationPolicy> PlayerPtr PlayerContainer<CreationPolicy>::Create()
{
	return CreationPolicy<Player>().Create();
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::GetUserList(std::vector<UserInfo> &List)
{
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
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::CheckUserList()
{
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

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::DeleteAllBySession(GSCLIENT *pSession)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;

		if(pPlayer == NULL)
		{
			++iter;
			continue;;
		}

		if(pPlayer->m_UnityServerId == pSession->GetId())
		{
			pSession->Close();

			iter = m_PlayerMap.erase(iter);
			continue;
		}

		++iter;
	}
}


template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::Print()
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second != NULL)
		{
			printf("%S\n",iter->second->m_Account.GetName().c_str());
		}
		++iter;
	}

	printf("\nasocket count %d\n",JGSERVER.GetClientMgr().GetActiveSocketCount());

	printf("Total Count %d\n",m_PlayerMap.size());
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length)
{
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
}


template<template<class T> class CreationPolicy> template<class TYPE> void PlayerContainer<CreationPolicy>::BroadCast(TYPE MSG)
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
}

template<template<class T> class CreationPolicy> void PlayerContainer<CreationPolicy>::OnResponse(LPVOID Data)
{
	GSObject *pData = (GSObject *)Data;

	switch(pData->GetId())
	{
	case DBP_GET_LOGIN_INFO_RE:
		{
			RequestPlayerAuth *pAuth = (RequestPlayerAuth *)Data;

			if(pAuth->Index == 0)
			{
				//pAuth->pClient->OnDisconnect();
				pAuth->pClient->Close();

				return ;
			}

			if(pAuth->pClient->GetConnected() == 0)
			{
				//pAuth->pClient->OnDisconnect();
				pAuth->pClient->Close();
				return ;
			}

			if( PLAYERMGR.Search(pAuth->Account) != NULL) //존재함
			{
				//pAuth->pClient->OnDisconnect();
				pAuth->pClient->Close();

				return ;
			}

			if(pAuth->Account.size() < 3)	//3자리 보다 커야한다.
			{
				//pAuth->pClient->OnDisconnect();
				pAuth->pClient->Close();

				printf("call bad account \n");

				return ;
			}

			memcpy(&pAuth->pClient->m_LoginContext.account, pAuth->Account.c_str(),sizeof(pAuth->pClient->m_LoginContext.account));
			pAuth->pClient->m_LoginContext.IsSendedAuth = true;
			pAuth->pClient->m_LoginContext.Time = GetTickCount();

			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("ChannelId",1)
			ADD_JSON_MEMBER("SessionId",(int)pAuth->pClient->GetId())

			ADD_JSON_MEMBER("Index", (int)pAuth->Index)

			ADD_JSON_WSTR_MEMBER("Account",pAuth->Account)

			ADD_JSON_WSTR_MEMBER("CharName",pAuth->CharName[0])
			END_JSON_MEMBER(UNITY.GetTCPSocket(),ID_FG_PKT_REGIST_PLAYER_REQ)
		
		}
		break;
	}
}