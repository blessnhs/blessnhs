#include "StdAfx.h"
#include "DBProcessContainer.h"

#include "../../../GSPacket/protocol.h"

	DBProcessContainer::DBProcessContainer(void)
	{
	}

	DBProcessContainer::~DBProcessContainer(void)
	{
	}

	bool DBProcessContainer::Add(DBPROCESS_CER_PTR &pObj)
	{
		if(m_List.find(pObj->GetId()) != m_List.end())
		{
		}

		m_List[pObj->GetId()] = pObj;

		return TRUE;
	}

	bool DBProcessContainer::Del(DBPROCESS_CER_PTR &pObj)
	{
		std::map<DWORD,DBPROCESS_CER_PTR>::iterator iter = m_List.find(pObj->GetId());
		if(iter != m_List.end())
		{
			m_List.erase(iter);
		}
		else 
			return FALSE;

		return TRUE;
	}

	DBPROCESS_CER_PTR DBProcessContainer::Search(DWORD Id)
	{
		std::map<DWORD,DBPROCESS_CER_PTR>::iterator iter = m_List.find(Id);
		if(iter != m_List.end())
		{
			return iter->second;
		}

		return DBPROCESS_CER_PTR();
	}

	void DBProcessContainer::Initialize()
	{
		SYSTEM_INFO SystemInfo; 
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;

		for(int i=0;i<dwThreadNum;i++)
		{
			DBPROCESS_CER_PTR pDBProcess(new DBPROCESS_CER);
			pDBProcess->SetId( i);

			Add(pDBProcess);
		}

		TryConnect();

	}


	void DBProcessContainer::TryConnect()
	{
		std::map<DWORD,DBPROCESS_CER_PTR>::iterator iter = m_List.begin();

		while(iter != m_List.end())
		{
			DBPROCESS_CER_PTR process = iter->second;

			process->Initalize(_T("GS_ACCOUNT"),_T("SQL_NHS"),_T("SQL_NHS"));

			iter++;
		}
	}

