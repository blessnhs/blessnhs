#pragma once

#include "IGSContainer.h"
#include "Group.h"
#include "SingleTonHolder.h"

#include <list>
#include <boost/shared_ptr.hpp>

namespace GSRelayServer	{	namespace GroupContainer	{

class GroupContainer : public IContainer<GROUP_PTR>, public Singleton<GroupContainer>
{
public:

	GroupContainer(void);
	~GroupContainer(void);

	virtual bool Add(GROUP_PTR &pObj);
	virtual bool Del(GROUP_PTR &pObj);
	virtual GROUP_PTR Search(DWORD Id);

	void Initialize();

	std::map<DWORD,GROUP_PTR> m_List;

	DWORD Index;
};

}	}

#define GROUPCONTAINER Singleton<GSRelayServer::GroupContainer::GroupContainer>::Instance()