inline std::string trim_right ( const std::string & source , const std::string & t = " " ) 
{
	std::string str = source; 
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> CDBProcess<CreationPolicy1,CreationPolicy2>::CDBProcess(void)
{
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> CDBProcess<CreationPolicy1,CreationPolicy2>::~CDBProcess(void)
{
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2>  boost::object_pool<ResponsePlayerAuth>	&CDBProcess<CreationPolicy1,CreationPolicy2>::GetResponsePool()
{
	return m_ResponsePlayerAuthPool;
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2>  boost::object_pool<RequestPlayerAuth>   &CDBProcess<CreationPolicy1,CreationPolicy2>::GetRequestPool()
{
	return m_RequestPlayerAuthPool;
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> BOOL CDBProcess<CreationPolicy1,CreationPolicy2>::Initalize(	WCHAR *m_szDSNAcc,WCHAR *m_szDSNGame,WCHAR *m_szUID,WCHAR *m_szPWD)
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

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> int CDBProcess<CreationPolicy1,CreationPolicy2>::SelectCharacterInfo(char *Account,ResponsePlayerAuth &pRes)
{
	if(m_GameDB->IsOpen()==false){ return _ERR_NETWORK; }

	CDBHandle		dbhandle(m_GameDB->m_hdbc, &(m_GameDB->m_csDBHandle));
	SQLHSTMT		hstmt = dbhandle.GetHandle();
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet;
	SQLRETURN		retcode = SQL_ERROR;
	TCHAR			szsql[1024];
	sprintf(szsql, ("SELECT * FROM Character(nolock) WHERE Account = \'%s\'"), Account);
	retcode = OdbcExecDirect(m_GameDB, hstmt, szsql);
	if (retcode==SQL_ERROR){ return _ERR_NETWORK; }

	char Name[MAX_CHAR_LENGTH];
	char _Account[MAX_CHAR_LENGTH];
	DWORD Index;

	int Cnt = 0;
	while ( true )
	{
		retcode = SQLFetch(hstmt);
		if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_C_LONG, &Index, 0, &cbParmRet);
			SQLGetData(hstmt, 2, SQL_C_CHAR, _Account, sizeof(_Account), &cbParmRet);
			SQLGetData(hstmt, 3, SQL_C_CHAR, Name, sizeof(Name), &cbParmRet);
	
			memcpy(pRes.CharName[Cnt],Name,sizeof(pRes.CharName[Cnt]));
			memcpy(pRes.AccoutName,_Account,sizeof(pRes.AccoutName));
			pRes.Index = Index;
		} 
		else { break; }
	}
	return _ERR_NONE;

}

/*
int		CDBProcess::ProcedureUserLogin(MW_PKT_USER_LOGIN* pv)
{
	if(m_AccountDB->IsOpen()==false){ return 1; }
	if(pv==NULL){ return 2; }
	
	int nRet = _ERR_NONE;
	
	CDBHandle	dbhandle(m_AccountDB->m_hdbc, &(m_AccountDB->m_csDBHandle));
	SQLHSTMT	hstmt = dbhandle.GetHandle();
	SQLRETURN	retcode = SQL_ERROR;
	TCHAR		szsql[1024]; memset(szsql, 0, sizeof(szsql));
	SQLSMALLINT		sParmRet=0;
	SQLLEN			cbParmRet=0;
	
	wsprintf(szsql, "{call SP_GW_GAME_LOGIN (\'%s\',\'%s\',%d,?)}", pv->szUserID, pv->szSessionKey, pv->wChannelID);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, 0,0, &sParmRet,0, &cbParmRet );
	if (retcode==SQL_ERROR)
	{
		SYSLOG().Write("error SQLBindParameter \n" ); 
		return 4; 
	}

	retcode = OdbcExecDirect(m_AccountDB, hstmt, szsql);
	if (retcode==SQL_ERROR)
	{
			SYSLOG().Write("error SQL_ERROR \n" ); 
			return 5;
	}
	
	if(sParmRet != 1)
	{ 
			SYSLOG().Write("error sParmRet \n" ); 
			return 6;
	}

	return nRet;
}*/

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> ResponsePlayerAuth *CDBProcess<CreationPolicy1,CreationPolicy2>::Create(ResponsePlayerAuth name)
{
	return CreationPolicy1<ResponsePlayerAuth>().Create(name);
}

template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> RequestPlayerAuth  *CDBProcess<CreationPolicy1,CreationPolicy2>::Create(RequestPlayerAuth name)
{
	return CreationPolicy1<RequestPlayerAuth>().Create(name);
}


template<template<class T> class CreationPolicy1,template<class T> class CreationPolicy2> SQLRETURN CDBProcess<CreationPolicy1,CreationPolicy2>::OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, WCHAR* query)
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