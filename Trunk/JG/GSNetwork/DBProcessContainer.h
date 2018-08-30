#pragma once

#include "IGSContainer.h"
#include "DBProcess.h"
#include "SingleTonHolder.h"

#include <list>

namespace GSNetwork	{	namespace DBProcessContainer	{

class DBProcessContainer : public IContainer<DBPROCESS_PTR>, public Singleton<DBProcessContainer>
{
public:

	DBProcessContainer(void);
	~DBProcessContainer(void);

	virtual bool Add(DBPROCESS_PTR &pObj);
	virtual bool Del(DBPROCESS_PTR &pObj);
	virtual DBPROCESS_PTR Search(DWORD Id);

	void TryConnect();

	void Initialize();

	std::map<DWORD,DBPROCESS_PTR> m_List;
};

}	}

#define DBPROCESSCONTAINER Singleton<GSNetwork::DBProcessContainer::DBProcessContainer>::Instance()