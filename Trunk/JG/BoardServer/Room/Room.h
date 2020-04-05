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

	struct RoomStock
	{
		string Name;
		string Passwd;
		WORD  MAX_PLAYER;
		WORD  GameType;
	
	};

#define COLS  20         // 20
#define	ROWS  20         // 20

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

	void SetState(State state);
	State GetState();

	WORD GetCurrPlayer();
	PLAYER_PTR GetOtherPlayer(DWORD INDEX);

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

	void SendToAll(WORD MainId, BYTE* Data, WORD Length);

	void ClearBoard();
	void UpdateBoard(int x, int y, eTeam team);
	bool CheckGameResult(int _x, int _y, eTeam _stone);
	
	concurrency::concurrent_unordered_map<DWORD, PLAYER_PTR>		m_PlayerMap;

private:


	CRITICAL_SECTION												m_PublicLock;

	State															m_State;

	eTeam															m_Board[COLS][ROWS];

};

typedef boost::shared_ptr<Room> RoomPtr;



#define ROOM		Room 
#define ROOM_PTR	RoomPtr 
