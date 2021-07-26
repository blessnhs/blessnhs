#pragma once

class Player;

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

	std::string id;
	std::string pwd;
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

class LoadDBRoomList : public GSObject
{
public:
	LoadDBRoomList(void) {}
	~LoadDBRoomList(void) {}
};

class CreateRooom : public GSObject
{
public:
	CreateRooom(void) {}
	~CreateRooom(void) {}

	string room_name;
	INT64 user_id;
	string user_name;
	boost::shared_ptr<Player> pPlayer;
};

class EnterRooomDB : public GSObject
{
public:
	EnterRooomDB(void) {}
	~EnterRooomDB(void) {}

	int room_id;
	INT64 user_id;
	string user_name;
};

class LeaveRoomDB : public GSObject
{
public:
	LeaveRoomDB(void) {}
	~LeaveRoomDB(void) {}

	int room_id;
	INT64 user_id;
	string user_name;
};

class RoomPassThrou : public GSObject
{
public:
	RoomPassThrou(void) {}
	~RoomPassThrou(void) {}

	int room_id;
	INT64 user_id;
	string user_name;
	string msg;
	string time;
};

class RequestPrayList : public GSObject
{
public:
	RequestPrayList(void) {}
	~RequestPrayList(void) {}
};


class RequestRegPray : public GSObject
{
public:
	RequestRegPray(void) {}
	~RequestRegPray(void) {}

	std::string name;
	std::string contents;
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

