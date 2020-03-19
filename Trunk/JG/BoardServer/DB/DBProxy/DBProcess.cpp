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
}

CDBProcessCer::~CDBProcessCer(void)
{
	DeleteCriticalSection(&m_CS);
}


BOOL CDBProcessCer::Initalize(	CHAR *m_szDSN,CHAR *m_szUID,CHAR *m_szPWD)
{
	m_pDB = new COdbc;
	if( m_pDB->Open2(m_szDSN, m_szUID, m_szPWD) == false )
	{
		printf(("fail -> %s(%ul)\n"), GetId());
		return false;
	}
	m_IsOpen = true;

	printf(("connected db (%lu)\n"), GetId());
	return true;
}

CDBProcessCer &GetDBProcess()
{
	static CDBProcessCer g_DBProcess;
	return g_DBProcess;
}
float CDBProcessCer::ProcedureVersion()
{
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szSQL[1024];
	SQLCHAR			szSessionKey[MAX_SESSION_KEY_SIZE] = { 0, };

	SQLREAL			sParmRet = 0;
	SQLLEN			cbParmRet = 0;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();


	sprintf_s((char*)szSQL, sizeof(szSQL),"{call SP_VERSION ( ?)}");
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, &sParmRet, 0, &cbParmRet);

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
		if (retcode == SQL_ERROR)
		{	// true is Disconnected...


			if (m_pDB->SQLErrorMsg(hstmt, __FUNCTION__) == true)
			{
				printf(("database server Network Error... %s() \n"), __FUNCTION__);
				m_pDB->Close();
				m_pDB->ReConnect();
				//odbc->SetRecconectFlag(true);
			}
			printf(("database server query Error... %s() \n"), __FUNCTION__);
			return -1;
		}
	}

	return sParmRet;
}

int	 CDBProcessCer::ProcedureUserLogout(const CHAR* id)
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szSQL[1024];
	sprintf_s((char*)szSQL, sizeof(szSQL) ,("{call SP_ACCOUNT_LOGOUT (\'%s\')}"), id);
	
	retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
	if (retcode == SQL_ERROR)
	{	// true is Disconnected...


		if (m_pDB->SQLErrorMsg(hstmt, __FUNCTION__) == true)
		{
			printf(("database server Network Error... %s() \n"), __FUNCTION__);
			m_pDB->Close();
			m_pDB->ReConnect();
			//odbc->SetRecconectFlag(true);
		}
		printf(("database server query Error... %s() \n"), __FUNCTION__);
		return _ERR_NETWORK;
	}
	

	return _ERR_NONE;
}

int  CDBProcessCer::DeleteAllConcurrentUser()
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szSQL[1024];
	sprintf_s((char*)szSQL, sizeof(szSQL) ,("{call SP_DELETE_ALL_CONCURRENT}"));

	retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
	if (retcode == SQL_ERROR)
	{	// true is Disconnected...


		if (m_pDB->SQLErrorMsg(hstmt, __FUNCTION__) == true)
		{
			printf(("database server Network Error... %s() \n"), __FUNCTION__);
			m_pDB->Close();
			m_pDB->ReConnect();
			//odbc->SetRecconectFlag(true);
		}
		printf(("database server query Error... %s() \n"), __FUNCTION__);
		return _ERR_NETWORK;
	}


	return _ERR_NONE;
}

int		CDBProcessCer::ProcedureUserLogin(const CHAR* id, const CHAR* pw, std::string& szKey, INT64 &Index)
{
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szSQL[1024];
	SQLCHAR			szSessionKey[MAX_SESSION_KEY_SIZE] = { 0, };
	SQLSMALLINT		sParmRet = 0;
	SQLLEN			cbParmRet = 0;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();


	sprintf_s((char*)szSQL, sizeof(szSQL),"{call SP_ACCOUNT_LOGIN (\'%s\', \'%s\', ? , ? ,?)}", id, pw);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sParmRet, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 64, 0, szSessionKey, sizeof(szSessionKey), &cbParmRet);
	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &Index, 0, &cbParmRet);

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
		if (retcode == SQL_ERROR)
		{	// true is Disconnected...


			if (m_pDB->SQLErrorMsg(hstmt, __FUNCTION__) == true)
			{
				printf(("database server Network Error... %s() \n"), __FUNCTION__);
				m_pDB->Close();
				m_pDB->ReConnect();
				//odbc->SetRecconectFlag(true);
			}
			printf(("database server query Error... %s() \n"), __FUNCTION__);
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

	printf("recv session key = %s id = %s\n", (char*)(LPCTSTR)szSessionKey, id);
	dbKey = trim_right(dbKey);

	szKey = dbKey;

	return _ERR_NONE;
}
