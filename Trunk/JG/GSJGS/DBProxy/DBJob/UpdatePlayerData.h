#pragma once

#include "GSObject.h"
#include "common.h"
#include <boost/shared_ptr.hpp>

#include "IMessage.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class UpdatePlayerData : public GSObject
{
public:
	UpdatePlayerData(void) 
	{
	
	}

	~UpdatePlayerData(void) {}
};

#define  UpdatePlayerDataPtr boost::shared_ptr<UpdatePlayerData>

