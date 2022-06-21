#include "StdAfx.h"
#include "GSServerAgent.h"

namespace GSNetwork	{	 

GSServerAgent::GSServerAgent(void)
{
}


GSServerAgent::~GSServerAgent(void)
{
}

bool GSServerAgent::Add(GSCLIENT_PTR pObj,DWORD Key)
{
	auto iter = m_ServerMap.find(Key);

	if(iter == m_ServerMap.end())
	{
		m_ServerMap[Key] = pObj;
		return true;;
	}
	return false;
}

bool GSServerAgent::Del(DWORD Key)
{
	auto iter = m_ServerMap.find(Key);

	if(iter != m_ServerMap.end())
	{
		m_ServerMap[Key] = NULL;
		return true;;
	}
	return false;
}

GSCLIENT_PTR GSServerAgent::Search(DWORD Id)
{
	auto iter = m_ServerMap.find(Id);

	if(iter != m_ServerMap.end())
	{
		return iter->second;

	}
	return NULL;
}

}	
