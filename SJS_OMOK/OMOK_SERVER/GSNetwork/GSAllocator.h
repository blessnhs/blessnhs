#pragma once


#include "GSFactory.h"
#include "TemplateStrategyPattern.h"

#include "GSTAllocator.h"
#include "SingleTonHolder.h"

namespace GSNetwork	{

class GSAllocator : public GSTAllocator , public Singleton<GSAllocator>
{
public:
	GSAllocator(void);
	~GSAllocator(void);
};

}

#define ALLOCATOR Singleton<GSNetwork::GSAllocator>::Instance()

