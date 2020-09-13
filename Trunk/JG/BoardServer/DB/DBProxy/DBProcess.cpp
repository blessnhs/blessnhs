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

	m_DB = new MongoDB("Board");
	
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


int  CDBProcessCer::RequestRank(std::list<Rank> &list)
{
	m_DB->RequestRank(list);
	return _ERR_NONE;
}

int  CDBProcessCer::CalcRank()
{
	m_DB->CalcRank();
	return _ERR_NONE;
}

int CDBProcessCer::UpdaetPlayerScore(INT64 Index,int Win, int Lose, int Draw,int Level,int Score)
{
	return m_DB->UpdaetPlayerScore(Index, Win, Lose, Draw, Level, Score);
}

bool CDBProcessCer::NickNameCheck(string Name, INT64 Index)
{
	return m_DB->IsExistNickName(Name, Index);
}


int CDBProcessCer::NoticeInfoInfo(string& notice)
{
	return m_DB->NoticeInfoInfo(notice);
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

int		CDBProcessCer::ProcedureUserLogin(const CHAR* flatformid, const int flatformtype, const CHAR* picture_url, const CHAR* email, const CHAR* locale,std::string& szKey, int& Rank, int& Score, int& Win, int& Lose, int& Draw, INT64& Index, int& Level,string& name)
{
	return m_DB->ProcedureUserLogin(flatformid, flatformtype, picture_url, email, locale, szKey, Rank, Score, Win, Lose, Draw, Index, Level, name);
}
