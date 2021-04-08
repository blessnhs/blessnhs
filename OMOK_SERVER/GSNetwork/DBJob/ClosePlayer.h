#pragma once

#include "common.h"
#include <boost/shared_ptr.hpp>

class ClosePlayer : public GSOBJECT
{
public:
	ClosePlayer(void) 
	{
		memset(this,0,sizeof(ClosePlayer));
	}

	~ClosePlayer(void) {}

	wstring AccountId;
};

#define  ClosePlayerPtr boost::shared_ptr<ClosePlayer>

