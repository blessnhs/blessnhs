#pragma once

class Player;

typedef GSNetwork::GSObject::GSObject GSObject;

class RequestPlayerAuth : public GSObject
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	std::string id;
	std::string pwd;
	std::string SessionKey;
	INT64 Index;
	int Result;
	int channel;
};
