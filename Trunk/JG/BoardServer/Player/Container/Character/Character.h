#pragma once

#include <windows.h>
#include <common.h>

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

private:

	std::string			m_Name;

	bool				m_Ready;
	bool				m_AllComplete;

	BYTE				m_Team;
};