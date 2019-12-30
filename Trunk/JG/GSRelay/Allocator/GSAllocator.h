#pragma once

#include <boost/pool/object_pool.hpp>

#include "GSFactory.h"
#include "TemplateStrategyPattern.h"

#include "GSTAllocator.h"
#include "SingleTonHolder.h"


class GSAllocator : public GSTAllocator , public Singleton<GSAllocator>
{
public:
	GSAllocator(void);
	~GSAllocator(void);


	template<class T>
	boost::shared_ptr<T> Create(Type2Type<T>)
	{
		static GSFactory<T,true>	m_Pool;
		return m_Pool.alloc();
	}

};

extern GSAllocator &GetAllocator();

#define ALLOCATOR GetAllocator()
