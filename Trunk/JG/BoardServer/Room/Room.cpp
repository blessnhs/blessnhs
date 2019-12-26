#include "StdAfx.h"
#include "./Room.h"

#include "../GSPacket/Front.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSSerialize.h"

DWORD Room::FindPlayer(PLAYER_PTR Player)
{
	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second == Player)
		{
			return iter->first;
		}

		iter++;
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
	EnterCriticalSection(&m_PublicLock);
	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second == Player)
		{
			iter = m_PlayerMap.erase(iter);
			LeaveCriticalSection(&m_PublicLock);
			return TRUE;
		}
		else
			iter++;
	}
	LeaveCriticalSection(&m_PublicLock);
	return FALSE;
}

bool Room::InsertPlayer(PLAYER_PTR Player)
{
	int MaxCount = m_Stock.MAX_PLAYER/2;

	if(MaxCount < 2) MaxCount = 2;

	EnterCriticalSection(&m_PublicLock);

	for(int i=0;i<MaxCount;i++)
	{
		if(m_PlayerMap.find(i) == m_PlayerMap.end())
		{
			m_PlayerMap[i] = Player;
			Player->m_Char[0].SetTeam( WHITE );
			Player->m_RoomNumber = GetId();
			LeaveCriticalSection(&m_PublicLock);
			return true;
		}
	}

	for(int i=0;i<MaxCount;i++)
	{
		if(m_PlayerMap.find(i) == m_PlayerMap.end())
		{
			m_PlayerMap[i+MaxCount] = Player;
			Player->m_Char[0].SetTeam( BLACK );
			LeaveCriticalSection(&m_PublicLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_PublicLock);
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

	EnterCriticalSection(&m_PublicLock);

	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if (iter->second != NULL)
		{
			GSCLIENT_PTR pPair = SERVER.GetClient(iter->second ->GetPair());
			if(pPair != NULL)
			{
				//if(Player->GetId() != iter->second->GetId())
				{
					FC_PKT_NEW_USER_IN_ROOM SND;

					SND.Pos = iter->first;
					SND.PlayerName = Player->m_Account.GetName().c_str();

					DECLARE_JSON_WRITER
					ADD_JSON_MEMBER("Pos",(WORD)SND.Pos)
					ADD_JSON_WSTR_MEMBER("PlayerName",SND.PlayerName)
					END_JSON_MEMBER(pPair->GetTCPSocket(),ID_FC_PKT_NEW_USER_IN_ROOM)
				}

			}
		}
		iter++;
	}

	LeaveCriticalSection(&m_PublicLock);
}

bool Room::IsAllComplete()
{
	EnterCriticalSection(&m_PublicLock);
	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if (iter->second != NULL)
		{
			if (iter->second->m_Char[0].GetAllComplete() == false)
			{
				LeaveCriticalSection(&m_PublicLock);
				return FALSE;
			}
		}
		iter++;
	}

	LeaveCriticalSection(&m_PublicLock);
	return TRUE;
}

