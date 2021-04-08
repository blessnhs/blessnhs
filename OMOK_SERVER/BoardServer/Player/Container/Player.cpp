#include "StdAfx.h"
#include "Player.h"

Player::Player(void)
{
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
	m_PairSessionId = 0;
}



