#include "StdAfx.h"
#include "./Room.h"

#include "../GSPacket/Front.h"
#include "./tank/IGameRule_Tank.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSSerialize.h"

namespace GSFrontServer	{	namespace	Room	{

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
	pGameRule = NULL;	
}

Room::~Room(void)
{
	if(pGameRule != NULL)
		delete pGameRule;
}

bool Room::RemovePlayer(PLAYER_PTR Player)
{
	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();
	while(iter != m_PlayerMap.end())
	{
		if(iter->second == Player)
		{
			iter = m_PlayerMap.erase(iter);
			return TRUE;
		}
		else
			iter++;
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
		pGameRule = (IGameRule::IGameRule *)new IGameRule::IGameRule_Tank::IGameRule_Tank;
		return TRUE;
	}

	return FALSE;
	
}


void Room::SendNewUserInfo(PLAYER_PTR Player)
{
	if(Player == NULL)	return ;

	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if (iter->second != NULL)
		{
			GSCLIENT *pPair = JGSERVER.GetClient(iter->second ->GetPair());
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
}

bool Room::IsAllComplete()
{
	std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		if (iter->second != NULL)
		{
			if(iter->second->m_Char[0].GetAllComplete() == false)
				return FALSE;
		}
		iter++;
	}

	return TRUE;
}

}	}