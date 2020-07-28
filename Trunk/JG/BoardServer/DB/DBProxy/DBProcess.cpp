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
		printf(("fail -> (%ul)\n"), GetId());
		return false;
	}
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


int  CDBProcessCer::RequestRank(std::list<Rank> &list)
{

	if (m_pDB->IsOpen() == false) { return _ERR_NETWORK; }

	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLSMALLINT		sParmRet = 0;
	SQLLEN			cbParmRet;
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szSQL[1024];

	sprintf_s((char*)szSQL, sizeof(szSQL), "SELECT * FROM Rank");
	retcode = SQLExecDirect(hstmt, szSQL, SQL_NTS);
	if (retcode == SQL_ERROR) { return _ERR_NETWORK; }

	while (true)
	{
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			Rank r;

			INT64 Index;
			int Rank;
			char Name[256];
			int Level;
			char pic[512];
			int win, lose, draw;

			SQLGetData(hstmt, 1, SQL_C_SLONG, &Rank, 0, &cbParmRet);
			SQLGetData(hstmt, 2, SQL_CHAR, Name, sizeof(Name), &cbParmRet);
			SQLGetData(hstmt, 3, SQL_C_LONG, &Level, 0, &cbParmRet);
			SQLGetData(hstmt, 4, SQL_CHAR, pic, sizeof(pic), &cbParmRet);

			SQLGetData(hstmt, 5, SQL_C_SBIGINT, &Index, 0, &cbParmRet);
			SQLGetData(hstmt, 6, SQL_C_SLONG, &win, 0, &cbParmRet);
			SQLGetData(hstmt, 7, SQL_C_SLONG, &lose, 0, &cbParmRet);
			SQLGetData(hstmt, 8, SQL_C_SLONG, &draw, 0, &cbParmRet);

			r.set_var_rank(Rank);
			r.set_var_name(Name);
			r.set_var_level(Level);
			r.set_var_pic_uri(pic);
			r.set_var_index(Index);
			r.set_var_win(win);
			r.set_var_lose(lose);
			r.set_var_draw(draw);

			list.push_back(r);

		}
		else { break; }
	}
	return _ERR_NONE;
}

int CDBProcessCer::UpdaetPlayerScore(INT64 Index,int Win, int Lose, int Draw,int Level,int Score)
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szSQL[1024];
	sprintf_s((char*)szSQL, sizeof(szSQL), ("{call SP_UPDATE_PLAYER_DATA (%d,%d,%d,%d,%d,%d)}"), Index,Win,Lose,Draw,Level,Score);

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


int CDBProcessCer::NoticeInfoInfo(string& notice)
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szsql[1024];
	SQLLEN			cbParmRet;

	sprintf_s((char*)szsql,1024, _T("SELECT Text FROM NOTICE"));
	retcode = SQLExecDirect(hstmt, szsql, SQL_NTS);
	if (retcode == SQL_ERROR) { return _ERR_NETWORK; }

	CHAR contents[2048];

	int Cnt = 0;
	while (true)
	{
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_CHAR, contents, sizeof(contents), &cbParmRet);
			notice = contents;
			break;
		}
		else { break; }
	}
	return _ERR_NONE;

}

int CDBProcessCer::UpdaetQNS(INT64 Index, std::string contents)
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szSQL[1024];
	sprintf_s((char*)szSQL, sizeof(szSQL), ("{call SP_ADD_QNS ('%s',%d)}"), contents.c_str(),Index);

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

int	 CDBProcessCer::ProcedureUserLogout(const DWORD id)
{
	SQLRETURN		retcode = SQL_ERROR;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLCHAR			szSQL[1024];
	sprintf_s((char*)szSQL, sizeof(szSQL) ,("{call SP_ACCOUNT_LOGOUT (%d)}"), id);
	
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

int		CDBProcessCer::ProcedureUserLogin(const CHAR* flatformid, const int flatformtype, const CHAR* name, const CHAR* picture_url, const CHAR* email,std::string& szKey, int& Rank, int& Score, int& Win, int& Lose, int& Draw, INT64& Index, int& Level)
{
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szSQL[1024];
	SQLCHAR			szSessionKey[MAX_SESSION_KEY_SIZE] = { 0, };
	SQLSMALLINT		sDBReturn = 0;
	SQLLEN			cbParmRet = 0;
	CDBHandle		dbhandle(m_pDB->m_hdbc, &(m_pDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();


	sprintf_s((char*)szSQL, sizeof(szSQL),"{call SP_ACCOUNT_LOGIN (\'%s\', %d,\'%s\',\'%s\',\'%s\', ? , ? , ? , ? , ? , ? , ? , ? , ? )}", flatformid, flatformtype,name, picture_url, email);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sDBReturn, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 64, 0, szSessionKey, sizeof(szSessionKey), &cbParmRet);

	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Rank, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 4, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Score, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 5, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Win, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 6, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Lose, 0, &cbParmRet);
	retcode = SQLBindParameter(hstmt, 7, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Draw, 0, &cbParmRet);

	retcode = SQLBindParameter(hstmt, 8, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &Level, 0, &cbParmRet);


	retcode = SQLBindParameter(hstmt, 9, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &Index, 0, &cbParmRet);

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

	if (sDBReturn != 0) {
		//if(sDBReturn == 1){}	// 아이디, 패스워드오류
		//if(sDBReturn == 2){}	// 이미 로그인 되어있음
		if (sDBReturn == 2) { return _ERR_LOGINED; }
		return _ERR_NOTREGISTERED;
	}
	std::string dbKey = (char*)(LPCTSTR)szSessionKey;

	printf("recv session key = %s id = %s\n", (char*)(LPCTSTR)szSessionKey, flatformid);
	dbKey = trim_right(dbKey);

	szKey = dbKey;

	return _ERR_NONE;
}
