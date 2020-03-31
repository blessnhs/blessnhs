#include "StdAfx.h"
#include "./Character.h"

Character::Character()
{
	m_AllComplete = false;
	m_RoomNumber = 0;

}

Character::~Character() {}

std::string& Character::GetName()
{
	return m_Name;	
}

void Character::SetName(std::string& _Name)
{
	m_Name =  _Name;
}


void Character::SetAllComplete(bool _ready)
{
	m_AllComplete = _ready;
}

bool Character::GetAllComplete()
{
	return m_AllComplete;
}


void Character::SetTeam(bool _team)
{
	m_Team = _team;
}

byte Character::GetTeam()
{
	return m_Team;
}

void Character::SetReady(bool _ready)
{
	m_Ready = _ready;
}

bool Character::GetReady()
{
	return m_Ready;
}


VOID Character::SetRoom(DWORD _id)
{
	m_RoomNumber = _id;
}

DWORD Character::GetRoom()
{
	return m_RoomNumber;
}

Score& Character::GetScore()
{
	return m_Score;
}
