#pragma once

#include "IManager.h"
#include "IFieldServerContainer.h"
#include "SingleTonHolder.h"

namespace GSFrontServer	{	 namespace FiledServer	{
		
class FiledServer : public IFieldServerContainer::IFieldServerContainer , public Singleton<FiledServer>
{
public:
	FiledServer(void);
	~FiledServer(void);

	bool Add(SESSION *pObj,DWORD Key);
	bool Del(DWORD Key);

	SESSION* Search(DWORD Id);

	std::map<DWORD,SESSION*> m_FieldServerMap;
};

}	}

#define FIELD_SVR Singleton<GSFrontServer::FiledServer::FiledServer>::Instance()