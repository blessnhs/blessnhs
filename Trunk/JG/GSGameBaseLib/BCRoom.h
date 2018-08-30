#pragma once
class BCRoom
{
public:
	BCRoom(void);
	~BCRoom(void);

	std::string m_Name; 
	std::string m_Passwd;

	std::vector<DWORD>  m_PlayerList;
};

