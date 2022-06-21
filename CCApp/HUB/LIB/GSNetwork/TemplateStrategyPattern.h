#pragma once

#include "TypeList.h"

template<class T>
struct OpNewCreator
{
	static T *Create()
	{
		return new T;
	}
protected:
	~OpNewCreator() {}
};

template<class T>
struct MallocCreator
{
	static T* Create()
	{
		void *buf = std::malloc(sizeof(T));
		if(!buf) return 0;

		return new(buf) T ;
	}
};

template<class T>
struct GSCreator
{
	boost::shared_ptr<T> Create()
	{
	    static GSFactory<T,true>		m_Pool;
		return m_Pool.alloc();
	}

};

template<class T>
struct GSCreatorArgument
{
	T* Create(T _name)
	{
		static boost::object_pool<T>	m_Pool;
		return m_Pool.malloc();
	}

};

template<class T>
struct TCreate
{
	T *Create(Type2Type<T>)
	{
		static boost::object_pool<T>	m_Pool;
		return m_Pool.malloc();
	}
};
