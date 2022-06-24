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

	std::string token;
	INT64 Index;
	int Result;
	int channel;

	DWORD ForntId;		//프런트 서버 아이디(어떤서버)
	DWORD FrontSid;		//프런트 서버안에서 어떤 소켓 클라이언트인지
};
