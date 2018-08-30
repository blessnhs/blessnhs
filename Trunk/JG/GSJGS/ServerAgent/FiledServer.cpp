#include "StdAfx.h"
#include "FiledServer.h"

namespace GSFrontServer	{	 namespace FiledServer	{

FiledServer::FiledServer(void)
{
}


FiledServer::~FiledServer(void)
{
}

bool FiledServer::Add(SESSION *pObj,DWORD Key)
{
	std::map<DWORD,SESSION*>::iterator iter = m_FieldServerMap.find(Key);

	if(iter == m_FieldServerMap.end())
	{
		m_FieldServerMap[Key] = (SESSION* )pObj;
		return true;;
	}
	return false;
}

bool FiledServer::Del(DWORD Key)
{
	std::map<DWORD,SESSION*>::iterator iter = m_FieldServerMap.find(Key);

	if(iter != m_FieldServerMap.end())
	{
		m_FieldServerMap.erase(Key);
		return true;;
	}
	return false;
}

SESSION* 
FiledServer::Search(DWORD Id)
{
	std::map<DWORD,SESSION*>::iterator iter = m_FieldServerMap.find(Id);

	if(iter != m_FieldServerMap.end())
	{
		return (SESSION *)(iter->second);

	}
	return NULL;
}

}	}
