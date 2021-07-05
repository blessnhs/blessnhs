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

	m_DB = new MongoDB("Hub");
	
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
