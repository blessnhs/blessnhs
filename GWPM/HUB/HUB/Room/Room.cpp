#include "StdAfx.h"
#include "./Room.h"
#include "../Server/GSHub.h"

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
			Player->m_Char[0].SetRoom(GetId());
			return true;
		}
	}

	return false;
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

				userinfo->set_var_index(Player->GetId());
				userinfo->set_var_name(Player->m_Account.GetName());
				userinfo->set_picture_uri(Player->m_Account.GetPicture_url());
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


WORD Room::GetCurrPlayer()
{
	int count = 0;
	for each (auto & player in m_PlayerMap)
	{
		if (player.second != NULL)
			count++;
	}
	return count;
}

void Room::SendToAll(WORD MainId, BYTE * Data, WORD Length)
{
	for each (auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		GSCLIENT_PTR pSession = SERVER.GetClient(iter.second->GetPair());

		if (pSession)
			pSession->GetTCPSocket()->WritePacket(MainId, 0, Data, Length);
	}
}
