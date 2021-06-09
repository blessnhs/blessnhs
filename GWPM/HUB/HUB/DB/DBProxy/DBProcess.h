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

	int	 ProcedureUserLogin(const std::string id, const std::string pwd, std::string& szKey, int& Score, INT64& Index, int& Level);


	int	 ProcedureUserLogout(const INT64 id);
	int  DeleteAllConcurrentUser();

	bool NickNameCheck(string Name, INT64 Index);

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

