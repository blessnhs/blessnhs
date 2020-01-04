#pragma once

#include "GSObject.h"
#include "../DBJob/DBContext.h"

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize(	WCHAR *m_szDSN,WCHAR *m_szUID,WCHAR *m_szPWD);

	int	 ProcedureUserLogin(const WCHAR* id, const WCHAR* pw, std::wstring&szKey, INT64 &Index);

	COdbc*				m_pDB;
	bool				m_IsOpen;

	CRITICAL_SECTION	m_CS;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

