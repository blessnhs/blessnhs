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
			if (iter->second->m_pDB == NULL || iter->second->m_pDB->IsOpen() == false)
			{
				return DBPROCESS_CER_PTR();
			}
			return iter->second;
		}

		return DBPROCESS_CER_PTR();
	}

	void DBProcessContainer::Initialize(INT Count)
	{
		for(int i=0;i< Count;i++)
		{
			DBPROCESS_CER_PTR pDBProcess(new DBPROCESS_CER);
			pDBProcess->SetId( i + MSG_TYPE_DB_1);

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

			process->Initalize(_T("board"),_T("blessnhs"),_T("blessnhs"));

			iter++;
		}
	}

