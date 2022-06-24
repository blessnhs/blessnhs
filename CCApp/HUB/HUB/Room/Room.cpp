#include "StdAfx.h"
#include "./Room.h"
#include "../Server/GSHub.h"


int  Room::FindPlayerPos(PLAYER_PTR Player)
{
	int pos = 0;
	for each (auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (iter.second->GetId() == Player->GetId())
		{
			return pos;
		}

		pos++;
	}

	return 0;
}

bool Room::FindPlayer(PLAYER_PTR Player)
{
	for each(auto iter in m_PlayerMap)
	{
		PlayerPtr pPlayer = iter.second;
		if (pPlayer == NULL)
			continue;

		if (iter.second->GetId() == Player->GetId())
		{
			return true;
		}
	}

	return false;
}

Room::Room(void) 
{
	InitializeCriticalSection(&m_PublicLock);
}

Room::~Room(void)
{
	m_MessageList.clear();

	DeleteCriticalSection(&m_PublicLock);
}

bool Room::RemovePlayer(PLAYER_PTR Player)
{
	m_PlayerMap[Player->GetId()] = NULL;
	Player->DelRoom(GetId());
	return FALSE;
}

bool Room::InsertPlayer(PLAYER_PTR Player)
{
	m_PlayerMap[Player->GetId()] = Player;
	Player->SetRoom(GetId());
	return true;
}

void Room::SendNewUserInfo(PLAYER_PTR Player,int RoomNumber)
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
				userinfo->set_var_room_number(RoomNumber);
				SEND_PROTO_BUFFER(nty, pPair, pPlayer->GetFrontSid())
			}
		}
	}
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

		GSCLIENT_PTR pSession = SERVER.GetClient(pPlayer->GetPair());

		if (pSession)
			pSession->GetTCPSocket()->WritePacket(MainId, 0, Data, Length, pPlayer->GetFrontSid());
	}
}


VOID Room::GetMessageList(google::protobuf::RepeatedPtrField<RoomMessage>* List)
{
	const int max_count = 10;
	int currcount = 0;

	for each (auto msg in m_MessageList)
	{
		RoomMessage* info = List->Add();
		info->mutable_var_name()->assign(msg.var_name());

		info->set_var_message(msg.var_message());
		info->set_var_message_int(msg.var_message_int());
		info->set_var_time(msg.var_time());
	}
}

void  Room::AddRoomMessage(RoomMessage msg)
{
	if (m_MessageList.size() > 100)
		m_MessageList.pop_front();

	m_MessageList.push_back(msg);
}
