#pragma once
#include "CppSQLite3.h"

class GwSqlLiteMgr 
{
//private:
public:
	CppSQLite3DB m_db;
	void makeNewDB();
	CRITICAL_SECTION m_dblock;
	char dbname[512];
	//BYTE dbflag;
	int memdbLen;
public:
	DWORD m_Tick;
	void writeChatLog(char* log);
	int SaveMemDB();
	GwSqlLiteMgr();
	~GwSqlLiteMgr();
	void UpdateData(char *Accountid,char* Data);
	bool SelectData(char *Accountid,char *Data);
	bool  Authentication(const char *Accountid,const char *passwd);

	CRITICAL_SECTION	m_Lock;

};

extern GwSqlLiteMgr &GetSqlLiteMgr();