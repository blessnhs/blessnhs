#include "StdAfx.h"
#include "GSAllocator.h"


GSAllocator::GSAllocator(void)
{
}


GSAllocator::~GSAllocator(void)
{
}


GSAllocator &GetAllocator()
{
	static GSAllocator g_allocator;
	return g_allocator;
}