#pragma once

#include <common.h>

#define MAX_LEVEL 18
#define POINT 25

class Score
{
public:
	Score() {}

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

	void AddScorePoint(int point)
	{
		m_Score += point;
	}

	int GetScorePoint()
	{
		return m_Score;
	}

	int SetScorePoint(int point)
	{
		m_Score = point;

		return m_Score;
	}

private:
	int m_Rank;
	int m_Score;
	int m_Win;
	int m_Lose;
	int m_Draw;
};

class Character
{
public:
	Character();
	~Character();

	std::string& GetName();
	void SetName(std::string& _Name);

	void SetReady(bool _ready);
	bool GetReady();

	void SetAllComplete(bool _ready);
	bool GetAllComplete();

	void SetTeam(bool _team);
	byte GetTeam();

	VOID SetRoom(DWORD _id);
	DWORD GetRoom();

	Score& GetScore();

	void SetLevel(int _level);
	byte GetLevel();

	void UpdateScore(int point);

private:

	std::string			m_Name;

	bool				m_Ready;
	bool				m_AllComplete;

	BYTE				m_Team;

	DWORD				m_RoomNumber;
	
	Score				m_Score;

	DWORD				m_Level;
};