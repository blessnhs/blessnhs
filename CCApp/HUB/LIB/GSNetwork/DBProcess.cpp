#include "StdAfx.h"
#include "DBProcess.h"

namespace GSNetwork	{	

inline std::string trim_right ( const std::string & source , const std::string & t = " " ) 
{
	std::string str = source; 
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

CDBProcess::CDBProcess(void)
{
	m_AccountDB = NULL;
	m_GameDB = NULL;

}

CDBProcess::~CDBProcess(void)
{
	if (m_AccountDB != NULL)
		delete m_AccountDB;

	if (m_GameDB != NULL)
		delete m_GameDB;

}

BOOL CDBProcess::Initalize(	CHAR *m_szDSNAcc,CHAR *m_szDSNGame,CHAR *m_szUID,CHAR *m_szPWD)
{
	m_AccountDB = new COdbc;
	//if( m_AccountDB->Open(m_szDSNAcc, m_szUID, m_szPWD) == false )
	if( m_AccountDB->Open2(m_szDSNAcc, m_szUID, m_szPWD) == false )
	{
		SYSLOG().Write("fail -> %s()\n", __FUNCTION__);
		return false;
	}
	
	m_GameDB = new COdbc;
	if( m_GameDB->Open2(m_szDSNGame, m_szUID, m_szPWD) == false )
	{
		SYSLOG().Write("fail -> %s()\n", __FUNCTION__);
		return false;
	}

	return true;
}

int CDBProcess::SelectCharacterInfo(const CHAR *Account,RequestPlayerAuth &pRes)
{
	if(m_GameDB->IsOpen()==false){ return _ERR_NETWORK; }

	CDBHandle		dbhandle(m_GameDB->m_hdbc, &(m_GameDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet;
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szsql[1024];
	sprintf_s((char*)szsql,1024, _T("SELECT * FROM Character WHERE Account = \'%s\'"), Account);
	retcode = OdbcExecDirect(m_GameDB, hstmt, szsql);
	if (retcode==SQL_ERROR){ return _ERR_NETWORK; }

	CHAR Name[MAX_CHAR_LENGTH];
	CHAR _Account[MAX_CHAR_LENGTH];
	DWORD Index;

	int Cnt = 0;
	while ( true )
	{
		retcode = SQLFetch(hstmt);
		if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_C_LONG, &Index, 0, &cbParmRet);
			SQLGetData(hstmt, 2, SQL_CHAR, _Account, sizeof(_Account), &cbParmRet);
			SQLGetData(hstmt, 3, SQL_CHAR, Name, sizeof(Name), &cbParmRet);
	
			pRes.CharName[Cnt] = Name;
			pRes.Account = _Account;
			pRes.Index = Index;
		} 
		else { break; }
	}
	return _ERR_NONE;

}

void CDBProcess::ClearConCurrentTable()
{
	if(m_AccountDB->IsOpen()==false){ return ; }

	int nRet = _ERR_NONE;

	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	SQLCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;

	sprintf_s((char*)szsql,1024, _T("delete GW_CONCURRENTUSER"));

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
		return ;
}

void CDBProcess::ClosePlayer(CHAR *Account)
{
	if(m_AccountDB->IsOpen()==false){ return ; }

	int nRet = _ERR_NONE;

	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	SQLCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;

	sprintf_s((char*)szsql,1024, _T("delete GW_CONCURRENTUSER where szAccountid = \'%s\'"), Account);

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
		return ;

}


int		CDBProcess::ProcedureUserLogin(const CHAR *Account,const CHAR *SessionKey,int ChannelId)
{
	if(m_AccountDB->IsOpen()==false){ return 1; }
	
	int nRet = _ERR_NONE;
	
	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	SQLCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;
	
	sprintf_s((char*)szsql,1024, _T("{call SP_GW_GAME_LOGIN (\'%s\',\'%s\',%d,?)}"),Account,SessionKey, ChannelId);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0,0, &sParmRet,0, &cbParmRet );
	if (retcode==SQL_ERROR)
	{
		SYSLOG().Write("error SQLBindParameter \n" ); 
		return 0; 
	}

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
	{
			SYSLOG().Write("error SQL_ERROR \n" ); 
			return 0;
	}
	
	if(sParmRet == 0)
	{ 
			SYSLOG().Write("error sParmRet \n" ); 
			return 0;
	}

	return sParmRet;
}

SQLRETURN CDBProcess::OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, SQLCHAR* query)
{
	SQLRETURN	res;
	res = SQLExecDirect(hstmt, query, SQL_NTS);
	if (res==SQL_ERROR)
	{
		int cnt = 20;		//쿼리 실패시 재 시도 횟수
		for(int i=0;i<cnt;i++)
		{
			res = SQLExecDirect(hstmt, query, SQL_NTS);
				
			if(res != SQL_ERROR)
			{
				return res;
			}
		}

		char err_str[256];
		if( odbc->SQLErrorMsg(hstmt,__FUNCTION__, err_str) == true ) 
		{
			while(!odbc->ReConnect())
			{
				Sleep(1000);
			}
			res = SQLExecDirect(hstmt, query, SQL_NTS);
				
			if(res != SQL_ERROR)
			{
				return res;
			}
		}
	}
	return res;
}

bool CDBProcess::RecordAuthenticKeyFromDB(CHAR* id, CHAR* key, CHAR* ip)
{
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szsql[1024];
	CDBHandle		dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();

	sprintf_s((char*)szsql,1024, _T("{call SP_GW_INSERT_UUID (\'%s\', \'%s\', \'%s\')}"), id, key, ip);
	retcode = SQLExecDirect (hstmt, szsql, SQL_NTS);
	if (retcode==SQL_ERROR)
	{
		if( m_AccountDB->SQLErrorMsg(hstmt, __FUNCTION__) == true ) 
		{	
			SYSLOG().Write(_T("database server Network Error... %s() \n"), __FUNCTION__);
			m_AccountDB->Close();
			m_AccountDB->ReConnect();
			//odbc->SetRecconectFlag(true);
		}
		SYSLOG().Write(_T("database server query Error... %s() \n"), __FUNCTION__);

		return false;
	}
	return true;
}


WORD CDBProcess::GetAuthenticKeyFromDB(const CHAR* id, const CHAR* pw,CHAR* out, const int outsize)
{
	SQLRETURN		retcode = SQL_ERROR;
	SQLCHAR			szSQL[1024];
	SQLCHAR			szSessionKey[MAX_SESSION_KEY_SIZE] = {0,};
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;
	CDBHandle		dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();


	sprintf_s((char*)szSQL,1024, _T("{call SP_GW_ACCOUNT_LOGIN (\'%s\', \'%s\', ? , ? )}"), id, pw);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0,0, &sParmRet,0, &cbParmRet );
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 64,0, szSessionKey, sizeof(szSessionKey), &cbParmRet );

	if(retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect (hstmt, szSQL,SQL_NTS);
		if (retcode==SQL_ERROR)
		{	// true is Disconnected...


			if( m_AccountDB->SQLErrorMsg(hstmt, __FUNCTION__) == true ) 
			{	
				SYSLOG().Write(_T("database server Network Error... %s() \n"), __FUNCTION__);
				m_AccountDB->Close();
				m_AccountDB->ReConnect();
				//odbc->SetRecconectFlag(true);
			}
			SYSLOG().Write(_T("database server query Error... %s() \n"), __FUNCTION__);
			return _ERR_NETWORK;
		}
	}

	if(sParmRet != 0) {
		//if(sParmRet == 1){}	// 아이디, 패스워드오류
		//if(sParmRet == 2){}	// 이미 로그인 되어있음
		if(sParmRet == 2){ return _ERR_LOGINED; }
		return _ERR_NOTREGISTERED;
	}
	std::string szKey = (char*)(LPCTSTR)szSessionKey;

	SYSLOG().Write("recv session key = %s id = %s\n",(char*)(LPCTSTR)szSessionKey,id);
	szKey = trim_right(szKey);


	_tcscpy_s(out, outsize, (char*)(LPCTSTR)szKey.c_str());

	//MultiByteToWideChar(CP_ACP,0, (char*)(LPCTSTR)szKey.c_str(), MAX_SESSION_KEY_SIZE, out, MAX_SESSION_KEY_SIZE);

	return _ERR_NONE;
}

}	