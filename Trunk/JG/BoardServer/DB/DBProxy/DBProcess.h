#pragma once

#include "GSObject.h"
#include "../DBJob/DBContext.h"

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize(	CHAR *m_szDSN,CHAR *m_szUID,CHAR *m_szPWD);

	int	 ProcedureUserLogin(const CHAR* id, const CHAR* pw, std::string&szKey, INT64 &Index);
	int	 ProcedureUserLogout(const CHAR* id);
	int  DeleteAllConcurrentUser();

	float ProcedureVersion();

	COdbc*				m_pDB;
	bool				m_IsOpen;

	CRITICAL_SECTION	m_CS;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

