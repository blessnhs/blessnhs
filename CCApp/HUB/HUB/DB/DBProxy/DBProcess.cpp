#include "StdAfx.h"
#include "DBProcess.h"

inline std::string trim_right ( const std::string & source , const std::string & t = " " ) 
{
	std::string str = source; 
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

CDBProcessCer::CDBProcessCer(void)
{
	m_IsOpen = false;
	InitializeCriticalSection(&m_CS);

	m_DB = new MongoDB("CCA");
	
}

CDBProcessCer::~CDBProcessCer(void)
{
	DeleteCriticalSection(&m_CS);

	delete m_DB;
}


BOOL CDBProcessCer::Initalize()
{
	m_IsOpen = true;
	return true;
}

CDBProcessCer &GetDBProcess()
{
	static CDBProcessCer g_DBProcess;
	return g_DBProcess;
}
float CDBProcessCer::ProcedureVersion()
{
	return m_DB->ProcedureVersion(1);
}

bool CDBProcessCer::NickNameCheck(string Name, INT64 Index)
{
	return m_DB->IsExistNickName(Name, Index);
}


std::list<tuple<__int64, string, string>> CDBProcessCer::NoticeInfoInfo()
{
	return m_DB->NoticeInfoInfo();
}

list<tuple<string, string,string>> CDBProcessCer::PrayList()
{
	return m_DB->PrayList();
}

int CDBProcessCer::RegPray(string name, string contents)
{
	return m_DB->UpdaetPray(name, contents);
}

int CDBProcessCer::UpdaetQNS(INT64 Index, std::string contents)
{
	return m_DB->UpdaetQNS(Index, contents);
}

int	 CDBProcessCer::ProcedureUserLogout(const INT64 Index)
{
	return m_DB->ProcedureUserLogout(Index);
}

int  CDBProcessCer::DeleteAllConcurrentUser()
{
	return m_DB->DeleteAllConcurrentUser();
}

int		CDBProcessCer::ProcedureUserLogin(const std::string id, const std::string pwd, std::string& szKey,int& Score, INT64& Index, int& Level)
{
	return m_DB->ProcedureUserLogin(id, pwd, szKey, Score, Index, Level);
}


int CDBProcessCer::InsertPurchase(int64_t INDEX, std::string purchase_id)
{
	return m_DB->InsertPurchase(INDEX, purchase_id);
}

std::list<string> CDBProcessCer::GetPurchaseList(int64_t INDEX)
{
	return m_DB->GetPurchaseList(INDEX);
}


int CDBProcessCer::DeleteCamera(int64_t INDEX, std::string machine_id)
{
	return m_DB->DeleteCamera(INDEX, machine_id);
}

int CDBProcessCer::RegisterCamera(int64_t INDEX, std::string machine_id, std::string machine_name)
{
	return m_DB->RegisterCamera(INDEX, machine_id, machine_name);
}

std::list<tuple<INT64, string, string>> CDBProcessCer::RegCameraList(int64_t INDEX, int count)
{
	return m_DB->RegCameraList(INDEX, count);
}


int CDBProcessCer::CreateRoom(string room_name, INT64 user_id, string user_name)
{
	return m_DB->CreateRoom(room_name,user_id, user_name);
}

int CDBProcessCer::EnterRoom(int room_id, INT64 user_id, string user_name)
{
	return m_DB->EnterRoom(room_id,user_id,user_name);
}

int CDBProcessCer::LeaveRoom(int room_id, INT64 user_id, string user_name)
{
	return m_DB->LeaveRoom(room_id,user_id,user_name);
}

int CDBProcessCer::AddRoomMessage(int room_id, INT64 user_id, string user_name, string msg,string time)
{
	return m_DB->AddRoomMessage(room_id, user_id, user_name, msg, time);
}

std::list<tuple<__int64, string, int, string, string>> CDBProcessCer::LoadRoomMessage(int room_id, int count)
{
	return m_DB->LoadRoomMessage(room_id, count);
}

std::list<tuple<int, string, string>> CDBProcessCer::LoadRooms(int count)
{
	return m_DB->LoadRooms(count);
}
