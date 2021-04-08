#include "../stdafx.h"
#include "GwSqlLiteMgr.h"
#include "CppSQLite3.h"
#include "sqlite3ext.h"
//#include "sqlite3.h"

//db는 이름이 바뀌지만
//테이블은 언제나 동일한 이름 아? 오? 아오 비데

GwSqlLiteMgr::GwSqlLiteMgr(void)
{
	memdbLen = (sizeof(BYTE)*1024*1024)*60;
//	dbflag = 0;
	memset(dbname,0x00,sizeof(dbname));
	InitializeCriticalSectionAndSpinCount(&m_Lock,0x80000400);
	makeNewDB();
	m_Tick = GetTickCount();
}

GwSqlLiteMgr::~GwSqlLiteMgr(void)
{
	DeleteCriticalSection(&m_Lock);
	m_db.close();
}

int GwSqlLiteMgr::SaveMemDB()
{
	//CThreadSync Sync;	

	if(m_db.mpDB==NULL)
		return 0;

	int rc=-1;                  
	sqlite3 *fileDB;           
	sqlite3_backup *pBackup;  

	char tmp[512];
	memset(tmp,0x00,sizeof(tmp));

	SYSTEMTIME time;   
	GetLocalTime(&time);

	sprintf_s(tmp,512,"MS_%d_%d_%d_%d_%d_%d.sqlite",time.wYear, time.wMonth, time.wDay,
	time.wHour, time.wMinute, time.wSecond);

	rc = sqlite3_open(tmp, &fileDB);
	if( rc==SQLITE_OK )
	{
		pBackup = sqlite3_backup_init(fileDB, "main", m_db.mpDB, "main");
		if( pBackup )
		{
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(fileDB);
	}
	
	(void)sqlite3_close(fileDB);

	m_db.close();
	return rc;
}

void GwSqlLiteMgr::makeNewDB()
{
	//dbflag^=1;

	//CThreadSync Sync;	
	
	//m_db.openMemDB();
	m_db.open("./MS.sqlite");
	if(m_db.tableExists("log")==false)
	{
		m_db.execDML("create table log(logdate date, accountid VARCHAR(32),data VARCHAR(512))");
	}

	if(m_db.tableExists("Users")==false)
	{
		m_db.execDML("create TABLE Users(Accountid VARCHAR PRIMARY KEY  NOT NULL , Passwd VARCHAR NOT NULL , Name VARCHAR, CP INTEGER)");
	}
}

bool GwSqlLiteMgr::SelectData(char *Accountid,char *Data)
{
	 char sQuery[512];
	 bool find = false;
	 
	 sprintf_s(sQuery,512,"SELECT* FROM log where accountid = '%s'",Accountid);

	 CppSQLite3Query pCountSql = m_db.execQuery(sQuery);
	 while( ! pCountSql.eof() )
	 { 
		  SYSLOG().Write("1.%s\n",pCountSql.fieldValue(0));
		  SYSLOG().Write("2.%s\n",pCountSql.fieldValue(1));
		  SYSLOG().Write("3.%s\n",pCountSql.fieldValue(2));
	  
		  pCountSql.nextRow();
		  find = true;
	 }

	 return find;
}

bool  GwSqlLiteMgr::Authentication(const char *Accountid,const char *passwd)
{
	SYSLOG().Write("authentication %s %s\n",Accountid,passwd);
	 char sQuery[512];
	 bool find = false;
	 sprintf_s(sQuery,512,"SELECT* FROM users where accountid = '%s' and Passwd = '%s'",Accountid,passwd);

	 CppSQLite3Query pCountSql = m_db.execQuery(sQuery);
	 while( ! pCountSql.eof() )
	 { 
		  SYSLOG().Write("1.%s\n",pCountSql.fieldValue(0));
		  SYSLOG().Write("2.%s\n",pCountSql.fieldValue(1));
		  SYSLOG().Write("3.%s\n",pCountSql.fieldValue(2));
	  
		  pCountSql.nextRow();
		  find = true;
	 }

	 return find;

}

void GwSqlLiteMgr::UpdateData(char *Accountid,char* Data)
{
	//CThreadSync Sync;	

	try
	{
		DWORD chkTick = GetTickCount() - m_Tick;

		if(chkTick>300000) //5분
		{
			__int64 memused = sqlite3_memory_used();//현재 사용량
			//__int64 highused = sqlite3_memory_highwater(1);//메모리 사용량 최고점

			SYSLOG().Write("sqlite mem used (%lld)\n", memused);

			if(memused>=memdbLen)
			{
				SaveMemDB();
				makeNewDB();
			}
			m_Tick = GetTickCount();
		}
		sqlite3_stmt *stmt;

		char tmp[512] = "insert into log values (DATETIME('now','localtime'), ?,?)";
//		const int a =1;
//		WCHAR string[512];

		sprintf_s(tmp,512,"insert into log values(DATETIME('now','localtime'), '%s','%s')",Accountid,Data);

		m_db.execDML(tmp);

	/*	int len = MultiByteToWideChar(CP_ACP, 0, Data, -1, NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, Data, -1, string, len);
		sqlite3_prepare(m_db.mpDB,tmp,strlen(tmp),&stmt,NULL);

		sqlite3_bind_text16(stmt, 0, Data, -1, SQLITE_STATIC);
		
		WCHAR Accountstring[512];
		len = MultiByteToWideChar(CP_ACP, 0, Accountid, -1, NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, Accountid, -1, Accountstring, len);

		sqlite3_bind_text16(stmt, 1, Accountstring, -1, SQLITE_STATIC);
	
		sqlite3_step(stmt);

		
		sqlite3_finalize(stmt);*/
	}
	catch (CppSQLite3Exception& e)
	{
		SYSLOG().Write("WriteMEMlogThreadProc:%d,%s", e.errorCode(), e.errorMessage());
	}
}

void GwSqlLiteMgr::writeChatLog(char* log)
{
	//CThreadSync Sync;	

	try
	{
		DWORD chkTick = GetTickCount() - m_Tick;

		if(chkTick>300000) //5분
		{
			__int64 memused = sqlite3_memory_used();//현재 사용량
			//__int64 highused = sqlite3_memory_highwater(1);//메모리 사용량 최고점

			SYSLOG().Write("sqlite mem used (%lld)\n", memused);

			if(memused>=memdbLen)
			{
				SaveMemDB();
				makeNewDB();
			}
			m_Tick = GetTickCount();
		}
		sqlite3_stmt *stmt;

		const char tmp[512] = "insert into chatlog values (DATETIME('now','localtime'), ?)";
		const int a =1;
		WCHAR string[512];

		int len = MultiByteToWideChar(CP_ACP, 0, log, -1, NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, log, -1, string, len);

		sqlite3_prepare(m_db.mpDB,tmp,(int)strlen(tmp),&stmt,NULL);
		sqlite3_bind_text16(stmt, 1, string, -1, SQLITE_STATIC);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	catch (CppSQLite3Exception& e)
	{
		SYSLOG().Write("WriteMEMlogThreadProc:%d,%s", e.errorCode(), e.errorMessage());
	}
}

GwSqlLiteMgr &GetSqlLiteMgr()
{
	static GwSqlLiteMgr g_sqlLiteMgr;
	return g_sqlLiteMgr;
}