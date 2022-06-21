#pragma once

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

	void SendNewUserInfo(PLAYER_PTR Player, int RoomNumber);

	bool FindPlayer(PLAYER_PTR Player);

	int  FindPlayerPos(PLAYER_PTR Player);

	bool RemovePlayer(PLAYER_PTR Player);

	bool IsAllComplete();

	WORD GetCurrPlayer();

	template<class T>
	void SendToAll(T& snd)
	{
		for each(auto iter in m_PlayerMap)
		{
			PlayerPtr pPlayer = iter.second;
			if (pPlayer == NULL)
				continue;

			GSCLIENT_PTR pSession = SERVER.GetClient(pPlayer->GetFront());

			if (pSession)
				SEND_PROTO_BUFFER2(pPlayer->GetFrontSid(),snd,pSession)
		}
	}

	template<class T>
	void SendToAll(T& snd,int PlayerId)
	{
		for each (auto iter in m_PlayerMap)
		{
			PlayerPtr pPlayer = iter.second;
			if (pPlayer == NULL)
				continue;

			if (pPlayer->GetId() == PlayerId)
				continue;

			GSCLIENT_PTR pSession = SERVER.GetClient(pPlayer->GetFront());

			if (pSession)
				SEND_PROTO_BUFFER2(pPlayer->GetFrontSid(), snd, pSession)
		}
	}

	VOID GetMessageList(google::protobuf::RepeatedPtrField<RoomMessage>* List);


	void SendToAll(WORD MainId, BYTE* Data, WORD Length);
	
	concurrency::concurrent_unordered_map<DWORD, PLAYER_PTR>		m_PlayerMap;

	void AddRoomMessage(RoomMessage msg);

private:


	list<RoomMessage> m_MessageList;

	CRITICAL_SECTION												m_PublicLock;

};

typedef boost::shared_ptr<Room> RoomPtr;



#define ROOM		Room 
#define ROOM_PTR	RoomPtr 
