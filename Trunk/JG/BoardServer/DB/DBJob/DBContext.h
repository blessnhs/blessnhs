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

class RequestPlayerScore : public GSObject
{
public:
	RequestPlayerScore(void) {}

	~RequestPlayerScore(void) {}

	int Win;
	int Lose;
	int Draw;
	DWORD Index;
	int Level;
	int Score;
};

class RequestRank : public GSObject
{
public:
	RequestRank(void) {}
	~RequestRank(void) {}
};


class RequestPlayerAuth : public GSObject
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	std::string Uid;
	std::string Token;
	std::string SessionKey;
	DWORD Index;
	int Result;
};

class RequestQNS : public GSObject
{
public:
	RequestQNS(void)
	{
	}

	~RequestQNS(void) {}

	std::string contents;
	DWORD Index;
	int Result;
};

class RequestLogout : public GSObject
{
public:
	RequestLogout(void) {}
	~RequestLogout(void) {}

	DWORD Index;
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

