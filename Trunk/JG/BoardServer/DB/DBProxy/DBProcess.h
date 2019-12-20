#pragma once

#include "GSObject.h"
#include "../DBJob/DBContext.h"

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize(	WCHAR *m_szDSN,WCHAR *m_szUID,WCHAR *m_szPWD);


	WORD GetAuthenticKeyFromDB(const WCHAR* id, const WCHAR* pw, WCHAR* out);	

	bool RecordAuthenticKeyFromDB( WCHAR* id,  WCHAR* key,  WCHAR* ip);
	int SelectCharacterInfo(const WCHAR *Account, RequestPlayerAuth &pRes);
	int	 ProcedureUserLogin(const WCHAR *Account, const WCHAR *SessionKey, int ChannelId);

	COdbc*				m_pDB;

	CRITICAL_SECTION	m_CS;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

