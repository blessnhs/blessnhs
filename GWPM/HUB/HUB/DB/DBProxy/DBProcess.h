#pragma once

using namespace std;

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize();

	int	 ProcedureUserLogin(const std::string id, const std::string pwd, std::string& szKey, int& Score, INT64& Index, int& Level);


	int	 ProcedureUserLogout(const INT64 id);
	int  DeleteAllConcurrentUser();

	bool NickNameCheck(string Name, INT64 Index);

	int UpdaetQNS(INT64 Index, string contents);
	std::list<tuple<__int64, string, string>> NoticeInfoInfo();
	float ProcedureVersion();


	list<tuple<string, string,string>> PrayList();
	int RegPray(string name, string contents);

	int CreateRoom(string room_name, INT64 user_id, string user_name);
	int EnterRoom(int room_id, INT64 user_id, string user_name);
	int LeaveRoom(int room_id, INT64 user_id, string user_name);
	int AddRoomMessage(int room_id, INT64 user_id, string user_name, string msg,string time);
	std::list<tuple<__int64, string, int, string, string>> LoadRoomMessage(int room_id, int count = 100);
	std::list<tuple<int, string, string>> LoadRooms(int count = 100);

	bool				m_IsOpen;

	CRITICAL_SECTION	m_CS;

	MongoDB				*m_DB;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

