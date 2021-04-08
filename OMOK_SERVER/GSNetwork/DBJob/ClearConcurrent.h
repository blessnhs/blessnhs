#pragma once

#include "common.h"
#include <boost/shared_ptr.hpp>

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

