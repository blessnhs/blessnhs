#pragma once

#include "GSObject.h"
#include "Character/Character.h"
					 
	
typedef GSNetwork::GSObject::GSObject GSObject;

class Score
{
public:
	Score()
	{

	}

	Score(int rank, int score, int win, int lose, int draw)
	{
		m_Rank = rank;
		m_Score = score;
		m_Win = win;
		m_Lose = lose;
		m_Draw = draw;

	}
	~Score() {}

	void SetScore(int rank, int score, int win, int lose, int draw)
	{
		m_Rank = rank;
		m_Score = score;
		m_Win = win;
		m_Lose = lose;
		m_Draw = draw;

	}

private:
	int m_Rank;
	int m_Score;
	int m_Win;
	int m_Lose;
	int m_Draw;
};

class Player : public GSObject 
{
public:

	Player(void);
	~Player(void);

	class Account
	{
	public:
		Account() {}
		~Account() {}

		std::string& GetName() { return m_Name;	}
		void SetName(const std::string& _Name) { m_Name = _Name; }

	private:

		std::string m_Name;
	};

	VOID SetChannel(DWORD _id);
	DWORD GetChannel();


	DWORD GetPair();
	void SetPair(DWORD pPair);

	void Initialize();

	VOID SetRoom(DWORD _id);
	DWORD GetRoom();

	Account		m_Account;
	Character	m_Char[MAX_CHAR_CNT];

	Score		m_Score;

private:

	DWORD       m_PairSessionId;
	DWORD		m_ChannelId;
	DWORD		m_RoomNumber;

};

typedef boost::shared_ptr<Player> PlayerPtr;


#define PLAYER		Player 
#define PLAYER_PTR	PlayerPtr 