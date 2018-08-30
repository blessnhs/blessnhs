#pragma once

#include "common.h"
#include <boost/shared_ptr.hpp>

namespace GSNetwork {


class RequestPlayerAuth : public GSOBJECT
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	std::wstring Account;
	std::wstring Passwd;
	std::wstring SessionKey;
	std::wstring CharName[MAX_CHAR_CNT];

	DWORD Index;
	int Result;
	

};

}
#define  RequestPlayerAuthPtr boost::shared_ptr<RequestPlayerAuth>

