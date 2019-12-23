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

int		CDBProcessCer::ProcedureUserLogin(const WCHAR* id, const WCHAR* pw, std::wstring& szKey, INT64 &Index)
{
	SQLRETURN		retcode = SQL_ERROR;
	TCHAR			szSQL[1024];
	SQLCHAR			szSessionKey[MAX_SESSION_KEY_SIZE] = { 0, };
	SQLSMALLINT		sParmRet = 0;
	SQLLEN			cbParmRet = 0;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();


	wsprintf(szSQL, _T("{call SP_ACCOUNT_LOGIN (\'%s\', \'%s\', ? , ? ,?)}"), id, pw);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sParmRet, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 64, 0, szSessionKey, sizeof(szSessionKey), &cbParmRet);
	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &Index, 0, &cbParmRet);

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
		if (retcode == SQL_ERROR)
		{	// true is Disconnected...


			if (m_pDB->SQLErrorMsg(hstmt, (LPCSTR)__FUNCTION__) == true)
			{
				wprintf(_T("database server Network Error... %s() \n"), __FUNCTION__);
				m_pDB->Close();
				m_pDB->ReConnect();
				//odbc->SetRecconectFlag(true);
			}
			wprintf(_T("database server query Error... %s() \n"), __FUNCTION__);
			return _ERR_NETWORK;
		}
	}

	if (sParmRet != 0) {
		//if(sParmRet == 1){}	// 아이디, 패스워드오류
		//if(sParmRet == 2){}	// 이미 로그인 되어있음
		if (sParmRet == 2) { return _ERR_LOGINED; }
		return _ERR_NOTREGISTERED;
	}
	std::string dbKey = (char*)(LPCTSTR)szSessionKey;

	printf("recv session key = %s id = %S\n", (char*)(LPCTSTR)szSessionKey, id);
	dbKey = trim_right(dbKey);

	WCHAR wideKey[MAX_SESSION_KEY_SIZE];

	MultiByteToWideChar(CP_ACP, 0, (char*)(LPCTSTR)dbKey.c_str(), MAX_SESSION_KEY_SIZE, wideKey, MAX_SESSION_KEY_SIZE);

	szKey = wideKey;

	return _ERR_NONE;
}
