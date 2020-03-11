#pragma once

#include "GSObject.h"
#include "common.h"
#include <boost/shared_ptr.hpp>
#include "IMessage.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class RequestVersion : public GSObject
{
public:
	RequestVersion(void) {}
	~RequestVersion(void) {}

	float Version;
};


class RequestPlayerAuth : public GSObject
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	std::wstring Account;
	std::wstring Passwd;
	std::wstring SessionKey;
	DWORD Index;
	int Result;
};



//sample
class RPA : public GSObject
{
public:
	RPA(void) 
	{
	}

	~RPA(void) {}

};


#define  RequestPlayerAuthPtr boost::shared_ptr<RequestPlayerAuth>

