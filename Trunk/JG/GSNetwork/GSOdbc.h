#pragma once

#include <windows.h>
#include <odbcinst.h> 
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string.h>
#include <odbcss.h>
#include <tchar.h>

#pragma comment(lib, "odbc32.lib")

#define SQLOK(nResult)	(nResult == SQL_SUCCESS || nResult == SQL_SUCCESS_WITH_INFO)
const unsigned long MAX_CONNECT	= 128;

class CDBHandle
{
public:
	CDBHandle(SQLHDBC hdbc, LPCRITICAL_SECTION pcs) 
	{
		m_hstmt = NULL;
		m_pCS = pcs;
		EnterCriticalSection(m_pCS);
		SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, hdbc, &m_hstmt);
	};
	CDBHandle(SQLHDBC hdbc) 
	{
		m_hstmt = NULL;
		m_pCS = NULL;
		SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, hdbc, &m_hstmt);
	};
	~CDBHandle() 
	{
		if(m_hstmt){ SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }
		if(m_pCS){ LeaveCriticalSection(m_pCS); }
	};

	inline SQLHSTMT GetHandle() { return m_hstmt; };
	/*void CloseHandle() 
	{ 
		if(m_hstmt)
		{ 
			SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, m_hstmt); 
			m_hstmt = NULL; 
		}
	};*/
private:
	SQLHSTMT			m_hstmt;
	LPCRITICAL_SECTION	m_pCS;
};

class COdbc
{
public:
	COdbc(void);
	virtual ~COdbc(void);

public:
	SQLHDBC	m_hdbc;
	CRITICAL_SECTION m_csDBHandle;
	
	inline bool IsOpen ();
	bool Open(LPCTSTR szDSN, LPCTSTR szUID, LPCTSTR szPWD, bool is_ipconnect=false);
	bool Open2(LPCTSTR odbcName, LPCTSTR loginName, LPCTSTR loginPwd);
	void Close();
	bool ReConnect();
	bool SQLErrorMsg(SQLHSTMT hstmt, LPCSTR errfunction, char* error_str);
	//bool SQLErrorMsg(SQLHSTMT hstmt, LPCSTR errfunction, int& error);
	bool SQLErrorMsg(SQLHSTMT hstmt, LPCSTR errfunction);
	bool SQLErrorMsg(SQLSMALLINT HandleType, SQLHANDLE Handle);

	inline bool GetRecconectFlag(){ return m_bReconnectFlag; }
	inline void SetRecconectFlag(bool v){ m_bReconnectFlag=v; }
	
private:
	SQLHENV m_henv;
	SQLCHAR m_szConnect[MAX_CONNECT];
	bool m_bReconnectFlag;

	bool Open(); 
};