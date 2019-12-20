#include "StdAfx.h"
#include "Player.h"

Player::Player(void)
{
	m_UnityServerId = 0;
	m_ChannelId = 1;
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

void Player::Account::SetName(std::wstring _Name)
{
	m_Name = _Name;
}

VOID Player::SetChannel(DWORD _id)
{
	m_ChannelId = _id;
}

DWORD Player::GetChannel()
{
	return m_ChannelId;
}

void Player::Initialize()
{
	memset(&m_Account,0,sizeof(m_Account));
	memset(&m_Char,0,sizeof(m_Char));

	m_GameServerId = 0;

	m_UnityServerId = 0;
	m_PairSessionId = 0;
	m_RoomNumber    = 0;
}

