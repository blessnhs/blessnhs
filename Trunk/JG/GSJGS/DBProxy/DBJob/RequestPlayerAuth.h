#pragma once

#include "GSObject.h"
#include "common.h"
#include <boost/shared_ptr.hpp>
#include "IMessage.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class RequestPlayerAuth : public GSObject
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	wstring Account;
	wstring SessionKey;
	wstring CharName[MAX_CHAR_CNT];
	DWORD Index;
	int Result;

	GSCLIENT	*pClient;
	

};

#define  RequestPlayerAuthPtr boost::shared_ptr<RequestPlayerAuth>

