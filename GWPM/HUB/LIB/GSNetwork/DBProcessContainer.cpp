#include "StdAfx.h"
#include "DBProcessContainer.h"

namespace GSNetwork	{	namespace DBProcessContainer	{

	DBProcessContainer::DBProcessContainer(void)
	{
	}

	DBProcessContainer::~DBProcessContainer(void)
	{
	}

	bool DBProcessContainer::Add(DBPROCESS_PTR &pObj)
	{
		if(m_List.find(pObj->GetId()) != m_List.end())
		{
		}

		m_List[pObj->GetId()] = pObj;

		return TRUE;
	}

	bool DBProcessContainer::Del(DBPROCESS_PTR &pObj)
	{
		std::map<INT64,DBPROCESS_PTR>::iterator iter = m_List.find(pObj->GetId());
		if(iter != m_List.end())
		{
			m_List.erase(iter);
		}
		else 
			return FALSE;

		return TRUE;
	}

	DBPROCESS_PTR DBProcessContainer::Search(INT64 Id)
	{
		std::map<INT64,DBPROCESS_PTR>::iterator iter = m_List.find(Id);
		if(iter != m_List.end())
		{
			return iter->second;
		}

		return DBPROCESS_PTR();
	}

	void DBProcessContainer::Initialize()
	{
//		for(int i=0;i<MAX_DB_CONNECTION_COUNT;i++)
		{
//			DBPROCESS_PTR pDBProcess(new DBPROCESS);
//			pDBProcess->SetId( GSMainProcess::GSMainProcess::MSG_TYPE_DB_1+i);

//			Add(pDBProcess);
		}

		TryConnect();

	}


	void DBProcessContainer::TryConnect()
	{
		std::map<INT64,DBPROCESS_PTR>::iterator iter = m_List.begin();

		while(iter != m_List.end())
		{
			DBPROCESS_PTR process = iter->second;

//			process->Initalize(IOCP.m_IniInfo.m_DSNAccount,IOCP.m_IniInfo.m_DSNGameDB,
//				IOCP.m_IniInfo.m_Account,IOCP.m_IniInfo.m_Passwd);

			iter++;
		}
	}

}	}