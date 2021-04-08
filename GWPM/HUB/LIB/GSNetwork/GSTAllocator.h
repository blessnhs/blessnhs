#pragma once
#include <boost/pool/object_pool.hpp>

#include "GSFactory.h"
#include "./IMessage.h"
#include "TemplateStrategyPattern.h"

class GSTAllocator
{
public:
	GSTAllocator(void);
	~GSTAllocator(void);


	template<class T>
	boost::shared_ptr<T> Create()
	{
		static GSFactory<T,true>	m_Pool;
		return m_Pool.alloc();
	}

	template< template <class T> class U,class N>
	boost::shared_ptr<U<N>> Create()
	{
		static GSFactory<U<N>,true>	m_Pool;
		return m_Pool.alloc();
	}

	template<class T>
	boost::object_pool<T>& GetObjectPool()
	{
		static boost::object_pool<T> obj_pool;
		return obj_pool;
	}
};

