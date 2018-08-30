#pragma once

#include "GSObject.h"
#include "common.h"
#include <boost/shared_ptr.hpp>

#include "IMessage.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class ClosePlayer : public GSObject
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

