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

	std::string Account;
	std::string Passwd;
	std::string SessionKey;
	DWORD Index;
	int Result;
};

class RequestLogout : public GSObject
{
public:
	RequestLogout(void) {}
	~RequestLogout(void) {}

	std::string Account;
};

class RequestDeleteAllConcurrentUser : public GSObject
{
public:
	RequestDeleteAllConcurrentUser(void) {}
	~RequestDeleteAllConcurrentUser(void) {}
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

