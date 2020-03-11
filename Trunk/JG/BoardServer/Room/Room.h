#pragma once

#include "GSObject.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include "GSClient.h"

#include "../Player/container/Player.h"
#include "../Server/GSBoard.h"
#include "MessageMapDef.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

using namespace std;
typedef GSNetwork::GSObject::GSObject GSObject;

	enum eTeam
	{
		WHITE = 0,
		BLACK
	};

	struct RoomStock
	{
		string Name;
		string Passwd;
		WORD  MAX_PLAYER;
		WORD  GameType;
	
	};

class Room : public GSObject 
{
public:
	Room(void);
	~Room(void);

	RoomStock	m_Stock;

	bool InsertPlayer(PLAYER_PTR Player);

	void SendNewUserInfo(PLAYER_PTR Player);

	DWORD FindPlayer(PLAYER_PTR Player);

	bool RemovePlayer(PLAYER_PTR Player);

	bool CreateGameRule(BYTE Type);

	bool IsAllComplete();

	WORD GetCurrPlayer()
	{
		int count = 0;
		for each (auto& player in m_PlayerMap)
		{
			if (player.second != NULL)
				count++;
		}
		return count;
	}

	template<class T>
	void SendToAll(T &snd)
	{
		for each(auto iter in m_PlayerMap)
		{
			PlayerPtr pPlayer = iter.second;
			if (pPlayer == NULL)
				continue;

			GSCLIENT_PTR pSession = SERVER.GetClient(pPlayer->GetPair());

			if (pSession)
				SEND_PROTO_BUFFER(snd,pSession)
		}
	}

	void SendToAll(WORD MainId,BYTE *Data,WORD Length)
	{
		for each(auto iter in m_PlayerMap)
		{
			PlayerPtr pPlayer = iter.second;
			if (pPlayer == NULL)
				continue;

			GSCLIENT_PTR pSession = SERVER.GetClient(iter.second->GetPair());

			if (pSession)
				pSession->GetTCPSocket()->WritePacket(MainId, 0, Data, Length);
		}
	}

	concurrency::concurrent_unordered_map<DWORD,PLAYER_PTR>		m_PlayerMap;

private:
	CRITICAL_SECTION											m_PublicLock;

};

typedef boost::shared_ptr<Room> RoomPtr;



#define ROOM		Room 
#define ROOM_PTR	RoomPtr 
