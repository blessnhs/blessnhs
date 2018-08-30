#pragma once

#include "GSObject.h"
#include "common.h"
#include <boost/shared_ptr.hpp>

#include "IMessage.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class ClearConcurrent : public GSObject
{
public:
	ClearConcurrent(void) 
	{
		
	}

	~ClearConcurrent(void) {}
};

#define  ClearConcurrentPtr boost::shared_ptr<ClearConcurrent>

