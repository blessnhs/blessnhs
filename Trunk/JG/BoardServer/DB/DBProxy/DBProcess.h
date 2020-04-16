#pragma once

#include "GSObject.h"
#include "../DBJob/DBContext.h"

#include "GS.CLI.pb.h"
#include "Enum.pb.h"

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize(	CHAR *m_szDSN,CHAR *m_szUID,CHAR *m_szPWD);

	int	 ProcedureUserLogin(const CHAR* flatformid, const int flatformtype, const CHAR* name, const CHAR* picture_url, const CHAR* email, 
		std::string&szKey,int &Rank, int& Score, int& Win, int& Lose, int& Draw,INT64 &Index, int& Level);


	int	 ProcedureUserLogout(const DWORD id);
	int  DeleteAllConcurrentUser();

	int  RequestRank(std::list<Rank>& list);

	int UpdaetPlayerScore(INT64 Index,int Win, int Lose, int Draw, int Level, int Score);

	float ProcedureVersion();

	COdbc*				m_pDB;
	bool				m_IsOpen;

	CRITICAL_SECTION	m_CS;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

