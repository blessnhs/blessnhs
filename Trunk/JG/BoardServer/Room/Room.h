#pragma once

#include "GSObject.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include "GSClient.h"

#include "../Player/container/Player.h"
#include "../Server/GSBoard.h"

using namespace std;
typedef GSNetwork::GSObject::GSObject GSObject;

	enum eTeam
	{
		WHITE = 0,
		BLACK
	};

	struct RoomStock
	{
		wstring Name;
		wstring Passwd;
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

	WORD GetCurrPlayer() { return (WORD)m_PlayerMap.size(); }

	template<class T>
	void SendToAll(T &snd)
	{
		std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();
		while(iter != m_PlayerMap.end())
		{
			GSCLIENT *pSession = IOCP.GetSessionMgr()->GetSession(iter->second->GetPair());
			Sender::Send(pSession,snd);
			iter++;
		}
	}

	void SendToAll(WORD MainId,BYTE *Data,WORD Length)
	{
		std::map<DWORD,PLAYER_PTR>::iterator iter = m_PlayerMap.begin();
		while(iter != m_PlayerMap.end())
		{
			GSCLIENT *pSession = SERVER.GetClient(iter->second->GetPair());
			pSession->GetTCPSocket()->WritePacket(MainId,0,Data,Length);
			iter++;
		}
	}

	std::map<DWORD,PLAYER_PTR>									m_PlayerMap;

private:
	CRITICAL_SECTION											m_PublicLock;

};

typedef boost::shared_ptr<Room> RoomPtr;



#define ROOM		Room 
#define ROOM_PTR	RoomPtr 
