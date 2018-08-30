#pragma once

struct Param
{
	std::wstring _string;
	byte Type;
};

class GSParam
{
public:
	GSParam(void);
	~GSParam(void);

	typedef std::vector< Param >	QUERY_PARAM;

	QUERY_PARAM	m_Input;
};

