#pragma once

namespace GSDBA	{	namespace CDBProcess	{

class CDBProcess
{
public:
	CDBProcess(void);
	~CDBProcess(void);
	
	BOOL Initalize(	WCHAR *m_szDSNAcc,WCHAR *m_szDSNGame,WCHAR *m_szUID,WCHAR *m_szPWD);

	int		ProcedureUserLogin();
	SQLRETURN	OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, WCHAR* query);

	COdbc*				m_AccountDB;
	COdbc*				m_GameDB;

};

extern CDBProcess &GetDBProcess();

}	}