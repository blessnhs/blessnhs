#include "StdAfx.h"
#include "./Room.h"

DWORD Room::FindPlayer(PLAYER_PTR Player)
{
	for each(auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (iter.second == Player)
		{
			return iter.first;
		}
	}

	return USHRT_MAX;
}

Room::Room(void) 
{
	InitializeCriticalSection(&m_PublicLock);
}

Room::~Room(void)
{
	DeleteCriticalSection(&m_PublicLock);
}

bool Room::RemovePlayer(PLAYER_PTR Player)
{
	for each(auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (iter.second == Player)
		{
			m_PlayerMap[iter.first] = NULL;
			return TRUE;
		}
	
	}

	return FALSE;
}

bool Room::InsertPlayer(PLAYER_PTR Player)
{
	int MaxCount = m_Stock.MAX_PLAYER/2;

	if(MaxCount < 2) MaxCount = 2;

	for(int i=0;i<MaxCount;i++)
	{
		if(m_PlayerMap.find(i) == m_PlayerMap.end())
		{
			m_PlayerMap[i] = Player;
			Player->m_Char[0].SetTeam( WHITE );
			Player->m_RoomNumber = GetId();
			return true;
		}
	}

	for(int i=0;i<MaxCount;i++)
	{
		if(m_PlayerMap.find(i) == m_PlayerMap.end())
		{
			m_PlayerMap[i+MaxCount] = Player;
			Player->m_Char[0].SetTeam( BLACK );
			return true;
		}
	}
	return false;
}

bool Room::CreateGameRule(BYTE Type)
{
	switch(Type)
	{
	case 0:
		return TRUE;
	}

	return FALSE;
	
}


void Room::SendNewUserInfo(PLAYER_PTR Player)
{
	if(Player == NULL)	return ;

	for each(auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (iter.second != NULL)
		{
			GSCLIENT_PTR pPair = SERVER.GetClient(iter.second->GetPair());
			if (pPair != NULL)
			{
				//if(Player->GetId() != iter->second->GetId())
				{
					/*					FC_PKT_NEW_USER_IN_ROOM SND;

										SND.Pos = iter->first;
										SND.PlayerName = Player->m_Account.GetName().c_str();

										DECLARE_JSON_WRITER
										ADD_JSON_MEMBER("Pos",(WORD)SND.Pos)
										ADD_JSON_WSTR_MEMBER("PlayerName",SND.PlayerName)
										END_JSON_MEMBER(pPair->GetTCPSocket(),ID_FC_PKT_NEW_USER_IN_ROOM)*/
				}

			}
		}

	}


}

bool Room::IsAllComplete()
{
	for each(auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;
		
		if (iter.second != NULL)
		{
			if (iter.second->m_Char[0].GetAllComplete() == false)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

