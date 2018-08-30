#include "StdAfx.h"
#include "Player.h"

Player::Player(void)
{
	m_DBStamp = 0;
	m_WillBeTerminated   = 0;
	FrontSession = NULL;

}


Player::~Player(void)
{
}
