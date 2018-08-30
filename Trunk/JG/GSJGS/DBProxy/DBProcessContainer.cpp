#include "StdAfx.h"
#include "DBProcessContainer.h"

#include "../GSPacket/protocol.h"

namespace GSFrontServer	{	namespace DBProcessContainer	{

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
		std::map<DWORD,DBPROCESS_PTR>::iterator iter = m_List.find(pObj->GetId());
		if(iter != m_List.end())
		{
			m_List.erase(iter);
		}
		else 
			return FALSE;

		return TRUE;
	}

	DBPROCESS_PTR DBProcessContainer::Search(DWORD Id)
	{
		std::map<DWORD,DBPROCESS_PTR>::iterator iter = m_List.find(Id);
		if(iter != m_List.end())
		{
			return iter->second;
		}

		return DBPROCESS_PTR();
	}

	void DBProcessContainer::Initialize()
	{
		SYSTEM_INFO SystemInfo; 
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadNum = 2*SystemInfo.dwNumberOfProcessors;


		for(int i=0;i<dwThreadNum;i++)
		{
			DBPROCESS_PTR pDBProcess(new DBPROCESS);
			pDBProcess->SetId( i);

			Add(pDBProcess);
		}

		TryConnect();

	}


	void DBProcessContainer::TryConnect()
	{
		std::map<DWORD,DBPROCESS_PTR>::iterator iter = m_List.begin();

		while(iter != m_List.end())
		{
			DBPROCESS_PTR process = iter->second;

			process->Initalize(INI.m_DSNAccount,INI.m_DSNGameDB,
				INI.m_Account,INI.m_Passwd);

			iter++;
		}
	}

}	}