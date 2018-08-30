#include "StdAfx.h"
#include "GSServerAgent.h"

namespace GSNetwork	{	 

GSServerAgent::GSServerAgent(void)
{
}


GSServerAgent::~GSServerAgent(void)
{
}

bool GSServerAgent::Add(GSCLIENT *pObj,DWORD Key)
{
	std::map<DWORD,GSCLIENT*>::iterator iter = m_ServerMap.find(Key);

	if(iter == m_ServerMap.end())
	{
		m_ServerMap[Key] = (GSCLIENT* )pObj;
		return true;;
	}
	return false;
}

bool GSServerAgent::Del(DWORD Key)
{
	std::map<DWORD,GSCLIENT*>::iterator iter = m_ServerMap.find(Key);

	if(iter != m_ServerMap.end())
	{
		m_ServerMap.erase(Key);
		return true;;
	}
	return false;
}

GSCLIENT* 
GSServerAgent::Search(DWORD Id)
{
	std::map<DWORD,GSCLIENT*>::iterator iter = m_ServerMap.find(Id);

	if(iter != m_ServerMap.end())
	{
		return (GSCLIENT *)(iter->second);

	}
	return NULL;
}

}	
