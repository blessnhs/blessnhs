#pragma once

#include "GSObject.h"
#include "GSClient.h"

class Player : public GSNetwork::GSObject::GSObject
{
public:
	Player(void);
	~Player(void);

	BYTE  m_Age;
	BOOL  m_Sex;
	DWORD m_Zone;
	DWORD m_X;
	DWORD m_Y;

	std::wstring CharName;
	std::wstring AccountName;

	DWORD	m_DBStamp;
	BOOL    m_WillBeTerminated;

	GSCLIENT *FrontSession;
	DWORD   m_SessionId;
};

typedef boost::shared_ptr<Player> PlayerPtr;
