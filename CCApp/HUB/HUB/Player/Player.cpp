#include "StdAfx.h"
#include "Player.h"

Player::Player(void)
{
	m_ChannelId = 1;
	m_FrontId = 0;
    m_FrontSid = 0;
	m_AliveTime = 0;
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
