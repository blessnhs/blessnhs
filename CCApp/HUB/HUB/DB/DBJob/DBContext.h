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

	std::string MachineId;
	std::string MachineModel;
	std::string Ip;

	DWORD ForntId;		//프런트 서버 아이디(어떤서버)
	DWORD FrontSid;		//프런트 서버안에서 어떤 소켓 클라이언트인지
};


class RequestVerifyPurchase : public GSObject
{
public:
	RequestVerifyPurchase(void)
	{
	}

	~RequestVerifyPurchase(void) {}

	std::string token;
	

	std::string PacketId;
	std::string PurchaseId;

	boost::shared_ptr<Player> pPlayer;

	DWORD ForntId;		//프런트 서버 아이디(어떤서버)
	DWORD FrontSid;		//프런트 서버안에서 어떤 소켓 클라이언트인지
};
