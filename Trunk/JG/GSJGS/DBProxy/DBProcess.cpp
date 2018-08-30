#include "StdAfx.h"
#include "DBProcess.h"

namespace GSFrontServer	{	namespace CDBProcess	{

inline std::string trim_right ( const std::string & source , const std::string & t = " " ) 
{
	std::string str = source; 
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

CDBProcess::CDBProcess(void)
{
}

CDBProcess::~CDBProcess(void)
{
}

BOOL CDBProcess::Initalize(	WCHAR *m_szDSNAcc,WCHAR *m_szDSNGame,WCHAR *m_szUID,WCHAR *m_szPWD)
{
	m_AccountDB = new COdbc;
	//if( m_AccountDB->Open(m_szDSNAcc, m_szUID, m_szPWD) == false )
	if( m_AccountDB->Open2(m_szDSNAcc, m_szUID, m_szPWD) == false )
	{
		printf("fail -> %s()\n", __FUNCTION__);
		return false;
	}
	
	m_GameDB = new COdbc;
	if( m_GameDB->Open2(m_szDSNGame, m_szUID, m_szPWD) == false )
	{
		printf("fail -> %s()\n", __FUNCTION__);
		return false;
	}

	return true;
}

int CDBProcess::SelectCharacterInfo(const WCHAR *Account,RequestPlayerAuth &pRes)
{
	if(m_GameDB->IsOpen()==false){ return _ERR_NETWORK; }

	CDBHandle		dbhandle(m_GameDB->m_hdbc, &(m_GameDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet;
	SQLRETURN		retcode = SQL_ERROR;
	WCHAR			szsql[1024];
	wsprintf(szsql, _T("SELECT * FROM Character WHERE Account = \'%s\'"), Account);
	retcode = OdbcExecDirect(m_GameDB, hstmt, szsql);
	if (retcode==SQL_ERROR){ return _ERR_NETWORK; }

	WCHAR Name[MAX_CHAR_LENGTH];
	WCHAR _Account[MAX_CHAR_LENGTH];
	DWORD Index;

	int Cnt = 0;
	while ( true )
	{
		retcode = SQLFetch(hstmt);
		if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_C_LONG, &Index, 0, &cbParmRet);
			SQLGetData(hstmt, 2, SQL_WCHAR, _Account, sizeof(_Account), &cbParmRet);
			SQLGetData(hstmt, 3, SQL_WCHAR, Name, sizeof(Name), &cbParmRet);
	
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
	TCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;

	wsprintf(szsql, _T("delete GW_CONCURRENTUSER"));

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
		return ;
}

void CDBProcess::ClosePlayer(WCHAR *Account)
{
	if(m_AccountDB->IsOpen()==false){ return ; }

	int nRet = _ERR_NONE;

	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	WCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;

	wsprintf(szsql, _T("delete GW_CONCURRENTUSER where szAccountid = \'%s\'"), Account);

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
		return ;

}


int		CDBProcess::ProcedureUserLogin(const WCHAR *Account,const WCHAR *SessionKey,int ChannelId)
{
	if(m_AccountDB->IsOpen()==false){ return 1; }
	
	int nRet = _ERR_NONE;
	
	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	WCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;
	
	wsprintf(szsql, _T("{call SP_GW_GAME_LOGIN (\'%s\',\'%s\',%d,?)}"),Account,SessionKey, ChannelId);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0,0, &sParmRet,0, &cbParmRet );
	if (retcode==SQL_ERROR)
	{
		printf("error SQLBindParameter \n" ); 
		return 0; 
	}

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
	{
			printf("error SQL_ERROR \n" ); 
			return 0;
	}
	
	if(sParmRet == 0)
	{ 
			printf("error sParmRet \n" ); 
			return 0;
	}

	return sParmRet;
}

SQLRETURN CDBProcess::OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, WCHAR* query)
{
	SQLRETURN	res;
	res = SQLExecDirect(hstmt, query, SQL_NTS);
	if (res==SQL_ERROR)
	{
		int cnt = 20;		//Äõ¸® ½ÇÆÐ½Ã Àç ½Ãµµ È½¼ö
		for(int i=0;i<cnt;i++)
		{
			res = SQLExecDirect(hstmt, query, SQL_NTS);
				
			if(res != SQL_ERROR)
			{
				return res;
			}
		}

		char err_str[256];
		if( odbc->SQLErrorMsg(hstmt, (LPCSTR)__FUNCTION__, err_str) == true ) 
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

}	}