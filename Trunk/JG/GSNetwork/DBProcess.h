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
		
	BOOL Initalize(	CHAR *m_szDSNAcc,CHAR *m_szDSNGame,CHAR *m_szUID,CHAR *m_szPWD);

	int	ProcedureUserLogin(const CHAR *Account,const CHAR *SessionKey,int ChannelId);
	int SelectCharacterInfo(const CHAR *Account,RequestPlayerAuth &pRes);
	void ClosePlayer(CHAR *Account);
	void ClearConCurrentTable();

	WORD GetAuthenticKeyFromDB(const CHAR* id, const CHAR* pw, CHAR* out,const int outsize);	

	bool RecordAuthenticKeyFromDB( CHAR* id,  CHAR* key,  CHAR* ip);

	SQLRETURN	OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, SQLCHAR* query);


	COdbc*				m_AccountDB;
	COdbc*				m_GameDB;
};

}	

#define DBPROCESS		GSNetwork::CDBProcess
#define DBPROCESS_PTR	boost::shared_ptr<DBPROCESS>
