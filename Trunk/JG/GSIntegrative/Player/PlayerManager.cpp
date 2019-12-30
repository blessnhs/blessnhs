#include "StdAfx.h"
#include "PlayerManager.h"

#include "../GSPacket/Front.h"
#include "../GSPacket/protocol.h"

PlayerManager::PlayerManager(void)
{
}


PlayerManager::~PlayerManager(void)
{
}

VOID PlayerManager::OnResponse(LPVOID Data)
{
	GSObject *pData = (GSObject *)Data;

	switch(pData->GetId())
	{
		case _PROTOCOL_DBA::DBP_GET_CHAR_INFO_RE:
		{
	
		}
		break;
	}
}

VOID PlayerManager::Print()
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer != NULL)
		{
			printf("%S\n",pPlayer->AccountName.c_str());
		}

		++iter;
	}

	printf("Total Count %d\n",m_PlayerMap.size());
}

VOID PlayerManager::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	switch(MainProtocol)
	{
	case 0:
		{
		
		}
	}
}

void PlayerManager::DestoryAll()
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer != NULL)
		{
			iter = m_PlayerMap.erase(iter);
			continue;
		}

		++iter;
	}

}

bool PlayerManager::Add(PlayerPtr &pObj)
{
	std::map<DWORD,PlayerPtr>::iterator iter;

	if(m_PlayerMap.size() == 0)
	{
		m_PlayerMap[pObj->GetId()] = pObj;
		return true;
	}

	iter = m_PlayerMap.find(pObj->GetId());

	if(iter == m_PlayerMap.end())
	{
		m_PlayerMap[pObj->GetId()] = pObj;
		return true;;
	}
	
	return false;
}

bool PlayerManager::Del(PlayerPtr &pObj)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.find(pObj->GetId());
	
	
	if(iter == m_PlayerMap.end())
	{
		return false;
	}

	m_PlayerMap.erase(iter);


	return true;
}

PlayerPtr PlayerManager::Search(DWORD Id)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.find(Id);
	
	if(iter == m_PlayerMap.end())
	{
		return PlayerPtr(NULL);
	}

	return (iter->second);
}

PlayerPtr PlayerManager::SearchBySession(DWORD id)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer != NULL)
		{
			if(pPlayer->m_SessionId == id)
				return pPlayer;
		}

		++iter;
	}

	return PlayerPtr(NULL);
}

PlayerPtr PlayerManager::Search(wstring Account)
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer->AccountName == Account)
		{
			return pPlayer;
		}

		++iter;
	}

	return PlayerPtr(NULL);
}

VOID PlayerManager::CheckCloseJob()
{
	std::map<DWORD,PlayerPtr>::iterator iter = m_PlayerMap.begin();

	while(iter != m_PlayerMap.end())
	{
		PlayerPtr pPlayer = iter->second;
		if(pPlayer != NULL)
		{
			if(pPlayer->m_WillBeTerminated == TRUE)
			{
				if(pPlayer->m_DBStamp <= 0)
				{
				/*	GF_PKT_CLOSE_PLAYER_RES snd;
					snd.Result = 0;
					snd.ChannelId = 0;
					memcpy(snd.Account, pPlayer->AccountName.c_str(),sizeof(snd.Account));
					memcpy(snd.CharName, pPlayer->CharName.c_str(),sizeof(snd.CharName));
					
					printf("CheckCloseJob CLOSE_PLAYER %s %s \n",snd.Account,snd.CharName);

					//GSServer::FrontAgency::GetFrontAgency().WritePacket(ID_GF_PKT_CLOSE_PLAYER_RES,0,(BYTE*)&snd,sizeof(snd));

					Sender::Send(pPlayer->FrontSession,snd);*/

					iter = m_PlayerMap.erase(iter);
					continue;
				} 
			}
		}

		++iter;
	}
}


GSFactory<Player, true> &PlayerManager::GetPlayerPool()
{
	return m_PlayerPool;
}


void PlayerManager::OnEvt(LPVOID Arg)
{

}
