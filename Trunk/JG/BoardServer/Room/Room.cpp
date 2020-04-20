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

	m_State = State::Prepare;
}

void Room::ClearBoard()
{
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

void Room::SetRoomState(State state)
{
	m_State = state;
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

PLAYER_PTR Room::GetOtherPlayer(DWORD INDEX)
{
	for each (auto & player in m_PlayerMap)
	{
		if (player.second == NULL)
			continue;

		if (player.second->GetId() == INDEX)
			continue;

		return player.second;

	}
	return NULL;
}

eTeam Room::GetTeamPlayer(DWORD INDEX)
{
	//매칭시 0번은 무조건 블랙 1번은 화이트로 처리하도록함
	int pos = 0;
	for each (auto & player in m_PlayerMap)
	{
		if (player.second == NULL)
			continue;

		if (player.second->GetId() != INDEX)
		{
			pos++;
			continue;
		}

		if(pos == 0)
			return eTeam::BLACK;
		else
			return eTeam::WHITE;

		pos++;

	}
	return eTeam::None;
}

void Room::RecoardResult(PLAYER_PTR Winner, PLAYER_PTR Loser)
{
	GAME_RESULT_NTY result_nty;
	result_nty.set_var_code(Success);


	//나중에 필요에 따라 프로시져를 하나로 통합해야 할것 같음
	//패배 증가
	{
		GSCLIENT_PTR pSession = SERVER.GetClient(Loser->GetPair());
		if (!pSession)
			return ;

		Loser->m_Char[0].UpdateScore(-POINT);

		boost::shared_ptr<RequestPlayerScore> pRequest = ALLOCATOR.Create<RequestPlayerScore>();
		pRequest->Index = Loser->GetId();
		pRequest->Win = 0;	pRequest->Lose = 1;	pRequest->Draw = 0; pRequest->Level = Loser->m_Char[0].GetLevel(); pRequest->Score = Loser->m_Char[0].GetScore().GetScorePoint();

		result_nty.set_var_index_1(Loser->GetId()); 
		result_nty.set_var_level_1(Loser->m_Char[0].GetLevel()); 
		result_nty.set_var_level_point_1(Loser->m_Char[0].GetScore().GetScorePoint());

		boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestPlayerScore>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestPlayerScore>>();
		PLAYER_MSG->pSession = pSession;
		PLAYER_MSG->pRequst = pRequest;
		PLAYER_MSG->Type = pSession->GetMyDBTP();
		PLAYER_MSG->SubType = ONQUERY;
		MAINPROC.RegisterCommand(PLAYER_MSG);
	}

	//승수 증가
	{
		result_nty.set_var_index(Winner->GetId());
		result_nty.set_var_name(Winner->m_Account.GetName());

		eTeam color = GetTeamPlayer(Winner->GetId());
		result_nty.set_var_color(color);


		Winner->m_Char[0].UpdateScore(POINT);

		GSCLIENT_PTR pSession = SERVER.GetClient(Winner->GetPair());
		if (pSession)
		{
			boost::shared_ptr<RequestPlayerScore> pRequest = ALLOCATOR.Create<RequestPlayerScore>();
			pRequest->Index = Winner->GetId();
			pRequest->Win = 1;	pRequest->Lose = 0;	pRequest->Draw = 0; pRequest->Level = Winner->m_Char[0].GetLevel(); pRequest->Score = Winner->m_Char[0].GetScore().GetScorePoint();

			result_nty.set_var_index_2(Winner->GetId());
			result_nty.set_var_level_2(Winner->m_Char[0].GetLevel());
			result_nty.set_var_level_point_2(Winner->m_Char[0].GetScore().GetScorePoint());

			boost::shared_ptr<Board::MSG_PLAYER_QUERY<RequestPlayerScore>>		PLAYER_MSG = ALLOCATOR.Create<Board::MSG_PLAYER_QUERY<RequestPlayerScore>>();
			PLAYER_MSG->pSession = pSession;
			PLAYER_MSG->pRequst = pRequest;
			PLAYER_MSG->Type = pSession->GetMyDBTP();
			PLAYER_MSG->SubType = ONQUERY;
			MAINPROC.RegisterCommand(PLAYER_MSG);
		}
	}

	SendToAll(result_nty);
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