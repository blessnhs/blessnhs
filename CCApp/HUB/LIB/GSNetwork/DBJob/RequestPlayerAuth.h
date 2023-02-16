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

	std::string Account;
	std::string Passwd;
	std::string SessionKey;
	std::string CharName[MAX_CHAR_CNT];

	DWORD Index;
	int Result;
	

};


}
#define  RequestPlayerAuthPtr boost::shared_ptr<RequestPlayerAuth>

