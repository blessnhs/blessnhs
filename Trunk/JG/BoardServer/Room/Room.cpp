#include "StdAfx.h"
#include "./Room.h"
#include "../Server/GSBoard.h"

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
	for(int i=0;i<m_Stock.MAX_PLAYER;i++)
	{
		if(m_PlayerMap.find(i) == m_PlayerMap.end())
		{
			m_PlayerMap[i] = Player;
			Player->m_Char[0].SetTeam( WHITE );
			Player->m_Char[0].SetRoom(GetId());
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
				NEW_USER_IN_ROOM_NTY nty;

				RoomUserInfo *userinfo = nty.mutable_var_room_user();

				std::string name;
				name.assign(Player->m_Account.GetName().begin(), Player->m_Account.GetName().end());

				userinfo->mutable_var_name()->assign(name);
			
				SEND_PROTO_BUFFER(nty, pPair)
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

