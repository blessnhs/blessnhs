#pragma once

#include "ACGameStock.h"
#include "TankGameStock.h"


#include <common.h>

class Character
{
public:
	Character();
	~Character();

	std::wstring& GetName();
	void SetName(wstring _Name);

	void SetReady(bool _ready);
	bool GetReady();

	void SetAllComplete(bool _ready);
	bool GetAllComplete();

	void SetTeam(bool _team);
	byte GetTeam();

private:

	std::wstring		m_Name;

	int			m_x;
	int			m_y;
	int			m_z;

	DWORD		m_ZoneNumber;
	bool		m_Ready;
	bool		m_AllComplete;

	ACGameStock	m_ACStock;
	TankGameStock m_TankStock;

	BYTE		m_Team;
};