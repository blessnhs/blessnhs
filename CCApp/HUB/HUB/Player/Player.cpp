#include "StdAfx.h"
#include "Player.h"

Player::Player(void)
{
	m_ChannelId = 1;
	m_FrontId = 0;
    m_FrontSid = 0;
	m_AliveTime = 0;
	m_DBId = 0;
}

Player::~Player(void)
{
}

DWORD Player::GetPair()
{
	return m_PairSessionId;
}

void Player::SetPair(DWORD pPair)
{
	m_PairSessionId = pPair;
}

VOID Player::SetChannel(DWORD _id)
{
	m_ChannelId = _id;
}


void Player::SetFront(DWORD _id)
{
	m_FrontId = _id;
}

DWORD Player::GetFront()
{
	return m_FrontId;
}

void Player::SetDBIndex(DWORD _id)
{
	m_DBId = _id;
}

DWORD Player::GetDBIndex()
{
	return m_DBId;
}

void Player::SetFrontSid(DWORD _id)
{
	m_FrontSid = _id;
}

DWORD Player::GetFrontSid()
{
	return m_FrontSid;
}



DWORD Player::GetChannel()
{
	return m_ChannelId;
}

void Player::Initialize()
{
	m_PairSessionId = 0;
}

VOID Player::DelRoom(DWORD _id)
{
	m_RoomNumbers.erase(_id);
}

VOID Player::SetRoom(DWORD _id)
{
	m_RoomNumbers.insert(_id);
}

std::set<DWORD> Player::GetRoom()
{
	return m_RoomNumbers;
}

std::string& Player::GetName()
{
	return m_Name;
}

void Player::SetName(std::string& _Name)
{
	m_Name = _Name;
}

std::string& Player::GetMachineName()
{
	return m_MachineName;
}

void Player::SetMachineName(std::string& _Name)
{
	m_MachineName = _Name;
}

std::string& Player::GetMachineId()
{
	return m_MachineId;
}

void Player::SetMachineId(std::string& _Id)
{
	m_MachineId = _Id;
}