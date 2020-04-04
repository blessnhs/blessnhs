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

	for (int i = 0; i < COLS; i++)
		for (int j = 0; j < ROWS; j++)
			m_Board[i][j] = None;
}

Room::~Room(void)
{
	DeleteCriticalSection(&m_PublicLock);
}

void Room::UpdateBoard(int x, int y, eTeam team)
{
	if (x < 0 || y < 0)
		return;

	if (x >= ROWS || y >= COLS)
		return;

	m_Board[y][x] = team;
}

bool  Room::CheckGameResult(int _x, int _y, eTeam _stone)   // 현재 놓은 돌의 좌표와 색상(1or2) 정보를 받음
{
	int x, y;
	int count;

	//가로
	x = _x;
	y = _y;
	count = 0;
	// x값만 감소시켜 stone과 다를때까지 감소
	while (x > 0 && m_Board[_y][ x - 1] == _stone)
		x--;

	// x값만 증가시켜 stone과 다를때까지 증가
	while (x < COLS && m_Board[_y][ x++] == _stone)
		count++;

	if (count == 5)  // x값을 5번 증가시켰을 경우 GameWin함수 호출.
		return true;

	//세로
	x = _x;
	y = _y;
	count = 0;
	// 이번에는 y값만 변화시켜 가로와 마찬가지로 오목을 체크한다.
	while (y > 0 && m_Board[y - 1][ _x] == _stone)
		y--;
	while (y < ROWS && m_Board[y++][ _x] == _stone)
		count++;

	if (count == 5)
		return true;

	//대각선↘
	x = _x;
	y = _y;
	count = 0;
	// x, y 모두 변화시켜 위와 같은 방법으로 오목을 체크.
	while (x > 0 && y > 0 && m_Board[y - 1][ x - 1] == _stone)
	{
		y--;
		x--;
	}

	while (x < COLS && y < ROWS && m_Board[y++][ x++] == _stone)
		count++;

	if (count == 5)
		return true;

	//대각선↙
	x = _x;
	y = _y;
	count = 0;
	// x, y 모두 변화시켜 위와 같은 방법으로 오목을 체크.
	while ((x + 1) < COLS && y > 0 && m_Board[y - 1][ x + 1] == _stone)
	{
		x++;
		y--;
	}
	while (x >= 0 && y < ROWS && m_Board[y++][x--] == _stone)
		count++;

	if (count == 5)
		return true;


	return false;
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

				userinfo->set_var_index(Player->GetId());
				userinfo->set_var_name(Player->m_Account.GetName());
			
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

void Room::SetState(State state)
{
	m_State = state;
}
State Room::GetState()
{
	return m_State;
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