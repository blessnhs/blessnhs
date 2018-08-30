#pragma once

#include "GSOdbc.h"

#include <boost/pool/object_pool.hpp>
#include "DBJob/RequestPlayerAuth.h"


namespace GSNetwork	{	

class CDBProcess : public GSOBJECT
{
public:
	CDBProcess(void);
	~CDBProcess(void);
		
	BOOL Initalize(	WCHAR *m_szDSNAcc,WCHAR *m_szDSNGame,WCHAR *m_szUID,WCHAR *m_szPWD);

	int	ProcedureUserLogin(const WCHAR *Account,const WCHAR *SessionKey,int ChannelId);
	int SelectCharacterInfo(const WCHAR *Account,RequestPlayerAuth &pRes);
	void ClosePlayer(WCHAR *Account);
	void ClearConCurrentTable();

	WORD GetAuthenticKeyFromDB(const WCHAR* id, const WCHAR* pw, WCHAR* out);	

	bool RecordAuthenticKeyFromDB( WCHAR* id,  WCHAR* key,  WCHAR* ip);

	SQLRETURN	OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, WCHAR* query);


	COdbc*				m_AccountDB;
	COdbc*				m_GameDB;
};

}	

#define DBPROCESS		GSNetwork::CDBProcess
#define DBPROCESS_PTR	boost::shared_ptr<DBPROCESS>
