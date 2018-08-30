#pragma once

#include "GSOdbc.h"
#include "GSObject.h"

#include <boost/pool/object_pool.hpp>
#include "DBJob/RequestPlayerAuth.h"


namespace GSFrontServer	{	namespace CDBProcess	{

class CDBProcess : public GSObject
{
public:
	CDBProcess(void);
	~CDBProcess(void);
		
	BOOL Initalize(	WCHAR *m_szDSNAcc,WCHAR *m_szDSNGame,WCHAR *m_szUID,WCHAR *m_szPWD);

	int	ProcedureUserLogin(const WCHAR *Account,const WCHAR *SessionKey,int ChannelId);
	int SelectCharacterInfo(const WCHAR *Account,RequestPlayerAuth &pRes);
	void ClosePlayer(WCHAR *Account);
	void ClearConCurrentTable();

	SQLRETURN	OdbcExecDirect(COdbc* odbc, SQLHSTMT	hstmt, WCHAR* query);


	COdbc*				m_AccountDB;
	COdbc*				m_GameDB;
};

}	}

#define DBPROCESS		GSFrontServer::CDBProcess::CDBProcess
#define DBPROCESS_PTR	boost::shared_ptr<DBPROCESS>
