#pragma once

using namespace std;

class MongoDB
{
public:
	MongoDB(string db_name)
	{
		default_db_name = db_name;
	}

	virtual ~MongoDB() {}

private:

	std::string default_db_name = "Antioch";

public:

	void CreateIndex();

	//버전 정보를 가져온다.
	double ProcedureVersion(int type);

	bool IsExistNickName(string name, int64_t Index);

	std::list<tuple<INT64, string, string>> NoticeInfoInfo();

	int UpdaetQNS(int64_t Index, std::string contents);

	int RegisterCamera(int64_t INDEX, std::string machine_id, std::string machine_name);
	int DeleteCamera(int64_t INDEX, std::string machine_id);


	int InsertPurchase(int64_t INDEX, std::string purchase_id);
	std::list<string> GetPurchaseList(int64_t INDEX);

	std::list<tuple<INT64, string, string>> RegCameraList(int64_t INDEX,int count = 5);

	std::list<tuple<string, string, string>> PrayList(int count = 100);

	int UpdaetPray(string name, string contents);

	int	 ProcedureUserLogout(const int64_t id);
	int  DeleteAllConcurrentUser();

	int64_t GetNextIndex();

	int		ProcedureUserLogin(std::string google_uid, std::string pwd, std::string& szKey, int& Score, INT64& Index, int& Level);

	int CreateRoom(string room_name, INT64& user_id, string user_name);

	int EnterRoom(int room_id, INT64& user_id, string user_name);

	int LeaveRoom(int room_id, INT64 user_id, string user_name);

	int AddRoomMessage(int room_id, INT64 user_id, string user_name, string msg, string time);

	std::list<tuple<__int64, string, int, string, string>> LoadRoomMessage(int room_id, int count = 100);

	std::list<tuple<int, string, string>> LoadRooms(int count = 100);
};
