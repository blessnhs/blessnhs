#pragma once

#include "IGSContainer.h"
#include "GSAgency.h"
#include "SingleTonHolder.h"

#include <list>

namespace GSServer	{	namespace GSContainer	{
	/*
class GSContainer : public IContainer<GSS>, public Singleton<GSContainer>
{
public:

	GSContainer(void);
	~GSContainer(void);

	virtual bool Add(GSS &pObj);
	virtual bool Del(GSS &pObj);
	virtual GSS Search(DWORD Id);

	void TryConnect();

	void Initialize();

	std::map<DWORD,GSS*> m_List;
};*/

}	}

#define GSCONTAINER Singleton<GSServer::FrontContainer::GSContainer>::Instance()