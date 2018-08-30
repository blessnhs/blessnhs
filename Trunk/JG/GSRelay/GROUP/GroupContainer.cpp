#include "StdAfx.h"
#include "GroupContainer.h"
#include "../GSIniContext.h"

namespace GSRelayServer	{	namespace GroupContainer	{

	GroupContainer::GroupContainer(void)
	{
		Index = 1;
	}

	GroupContainer::~GroupContainer(void)
	{
	}

	bool GroupContainer::Add(GROUP_PTR &pObj)
	{
		pObj->m_Stock.Id = Index;
		if(m_List.find(pObj->m_Stock.Id) != m_List.end())
		{

		}

		m_List[pObj->m_Stock.Id] = pObj;


		return TRUE;
	}

	bool GroupContainer::Del(GROUP_PTR &pObj)
	{
		std::map<DWORD,GROUP_PTR>::iterator iter = m_List.find(pObj->m_Stock.Id);
		if(iter != m_List.end())
		{
			m_List.erase(iter);
		}
		else 
			return FALSE;

		return TRUE;
	}

	GROUP_PTR GroupContainer::Search(DWORD Id)
	{
		std::map<DWORD,GROUP_PTR>::iterator iter = m_List.find(Id);
		if(iter != m_List.end())
		{
			return (iter->second);
		}
		return GROUP_PTR();
	}

	void GroupContainer::Initialize()
	{
		for(int i=0;i<INICONTEXT.m_UdpCount;i++)
		{
			GROUP *pGroup = new GROUP();
			pGroup->m_Stock.Id = i;
			GROUP_PTR pGroupPtr(pGroup);
			Add(pGroupPtr);
		}
	}

}	}