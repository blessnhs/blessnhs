#include "StdAfx.h"
#include "DBProcess.h"

inline std::string trim_right ( const std::string & source , const std::string & t = " " ) 
{
	std::string str = source; 
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

CDBProcessCer::CDBProcessCer(void)
{
	InitializeCriticalSection(&m_CS);
}

CDBProcessCer::~CDBProcessCer(void)
{
	DeleteCriticalSection(&m_CS);
}


BOOL CDBProcessCer::Initalize(	WCHAR *m_szDSN,WCHAR *m_szUID,WCHAR *m_szPWD)
{
	m_pDB = new COdbc;
	if( m_pDB->Open2(m_szDSN, m_szUID, m_szPWD) == false )
	{
		printf("fail -> %s()\n", __FUNCTION__);
		return false;
	}

	return true;
}

CDBProcessCer &GetDBProcess()
{
	static CDBProcessCer g_DBProcess;
	return g_DBProcess;
}

bool CDBProcessCer::RecordAuthenticKeyFromDB(WCHAR* id, WCHAR* key, WCHAR* ip)
{
	SQLRETURN		retcode = SQL_ERROR;
	TCHAR			szsql[1024]; 
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();

	wsprintf(szsql, _T("{call SP_GW_INSERT_UUID (\'%s\', \'%s\', \'%s\')}"), id, key, ip);
	retcode = SQLExecDirect (hstmt, szsql, SQL_NTS);
	if (retcode==SQL_ERROR)
	{
		if( m_pDB->SQLErrorMsg(hstmt, (LPCSTR)__FUNCTION__) == true ) 
		{	
			wprintf(_T("database server Network Error... %s() \n"), __FUNCTION__);
			m_pDB->Close();
			m_pDB->ReConnect();
			//odbc->SetRecconectFlag(true);
		}
		wprintf(_T("database server query Error... %s() \n"), __FUNCTION__);
		
		return false;
	}
	return true;
}


WORD CDBProcessCer::GetAuthenticKeyFromDB(const WCHAR* id, const WCHAR* pw,WCHAR* out)
{
	

	return _ERR_NONE;
}

int		CDBProcessCer::ProcedureUserLogin(const WCHAR *Account, const WCHAR *SessionKey, int ChannelId)
{
	int nRet = _ERR_NONE;


	return nRet;
}

int CDBProcessCer::SelectCharacterInfo(const WCHAR *Account, RequestPlayerAuth &pRes)
{
	if (m_pDB->IsOpen() == false) { return _ERR_NETWORK; }

	
	return _ERR_NONE;

}
