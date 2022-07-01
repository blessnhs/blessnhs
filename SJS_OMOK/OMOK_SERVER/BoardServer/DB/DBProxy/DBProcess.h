#pragma once

#include "GSObject.h"
#include "../DBJob/DBContext.h"

#include "GS.CLI.pb.h"
#include "Enum.pb.h"

using namespace std;

class CDBProcessCer : public GSNetwork::GSObject::GSObject
{
public:
	CDBProcessCer(void);
	~CDBProcessCer(void);
	
	BOOL Initalize();

	int	 ProcedureUserLogin(const CHAR* flatformid, const int flatformtype, const CHAR* picture_url, const CHAR* email, const CHAR* locale,
	std::string&szKey,int &Rank, int& Score, int& Win, int& Lose, int& Draw,INT64 &Index, int& Level,string& name);


	int	 ProcedureUserLogout(const INT64 id);
	int  DeleteAllConcurrentUser();

	int  RequestRank(std::list<Rank>& list);
	int  CalcRank();

	bool NickNameCheck(string Name, INT64 Index);

	int UpdaetPlayerScore(INT64 Index,int Win, int Lose, int Draw, int Level, int Score);

	int UpdaetQNS(INT64 Index, string contents);
	int NoticeInfoInfo(string& notice);
	float ProcedureVersion();

	bool				m_IsOpen;

	CRITICAL_SECTION	m_CS;

	MongoDB				*m_DB;

};

#define DBPROCESS_CER CDBProcessCer
#define DBPROCESS_CER_PTR boost::shared_ptr<DBPROCESS_CER>

extern CDBProcessCer &GetDBProcess();

