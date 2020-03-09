#pragma once

#include <windows.h>
#include <common.h>

class Character
{
public:
	Character();
	~Character();

	std::wstring& GetName();
	void SetName(std::wstring& _Name);

	void SetReady(bool _ready);
	bool GetReady();

	void SetAllComplete(bool _ready);
	bool GetAllComplete();

	void SetTeam(bool _team);
	byte GetTeam();

private:

	std::wstring		m_Name;

	bool				m_Ready;
	bool				m_AllComplete;

	BYTE				m_Team;
};