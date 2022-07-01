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

class NickNameCheck : public GSObject
{
public:
	NickNameCheck(void) {}
	~NickNameCheck(void) {}

	string NickName;
	INT64 Index;
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

class CalcRank : public GSObject
{
public:
	CalcRank(void) {}
	~CalcRank(void) {}
};


class RequestNotice : public GSObject
{
public:
	RequestNotice(void)
	{
	}

	~RequestNotice(void) {}
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
	INT64 Index;
	int Result;
	int channel;
};

class RequestQNS : public GSObject
{
public:
	RequestQNS(void)
	{
	}

	~RequestQNS(void) {}

	std::string contents;
	INT64 Index;
	int Result;
};

class RequestLogout : public GSObject
{
public:
	RequestLogout(void) {}
	~RequestLogout(void) {}

	INT64 Index;
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

