#pragma  once
#include <boost/shared_ptr.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/bind.hpp>


template<typename T,bool MultiThread = false>
class GSFactory 
{
public:
	GSFactory()
	{
		m_AllocCount = 0;
	}

	~GSFactory() 
	{
	}

	boost::shared_ptr<T> Create()
	{
		return alloc();
	}

	boost::shared_ptr<T> Malloc()
	{
		return Malloc();
	}

	boost::shared_ptr<T> alloc()
	{
		m_AllocCount++;

		if((m_AllocCount % 100 ) == 0)
		{
		//	SYSLOG().Write("alloc %s %d\n",typeid(T).name(),m_AllocCount);
		};

		return boost::shared_ptr<T> (
			m_allocator.construct(),
			boost::bind(&GSFactory::Destroy,this,_1));
	}

private:

	void Destroy(T *point)
	{
		m_allocator.destroy(point);

		if((m_AllocCount % 10 ) == 0)
		{
		//	SYSLOG().Write("dealloc %s %d\n",typeid(T).name(),m_AllocCount);
		};

		m_AllocCount--;
	}

	boost::object_pool<T> m_allocator;
	int  m_AllocCount;
};

template<typename T>
class GSFactory<T,true> : public GSGuard< GSFactory<T,true> >
{
public:
	GSFactory()
	{
		m_AllocCount = 0;
	}

	~GSFactory() 
	{
	}

	boost::shared_ptr<T> Create()
	{
		CThreadSync Sync;
		return alloc();
	}

	boost::shared_ptr<T> Malloc()
	{
		CThreadSync Sync;
		return Malloc();
	}

	boost::shared_ptr<T> alloc()
	{
		CThreadSync Sync;

		m_AllocCount++;

		if(m_AllocCount != 0 && (m_AllocCount % 100 ) == 0)
		{
		//	SYSLOG().Write("alloc %s %d\n",typeid(T).name(),m_AllocCount);
		};

	//	SYSLOG().Write("alloc %s %d\n",typeid(T).name(),m_AllocCount);

		return boost::shared_ptr<T> (
			m_allocator.construct(),
			boost::bind(&GSFactory::Destroy,this,_1));
	}

private:

	void Destroy(T *point)
	{
		CThreadSync Sync;
		m_allocator.destroy(point);

		m_AllocCount--;


		if (m_AllocCount != 0 && (m_AllocCount % 100) == 0)
		{
		//	SYSLOG().Write("dealloc %s %d\n", typeid(T).name(), m_AllocCount);
		};
	}

	boost::object_pool<T>	m_allocator;
	int						m_AllocCount;
};