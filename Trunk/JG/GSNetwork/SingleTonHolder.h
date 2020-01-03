#pragma once

template<typename T>
class SingleThreaded {
public : 
	typedef T VolatileType;

protected : 
	inline explicit SingleThreaded() {}
	inline ~SingleThreaded() {}

protected : 
	class LockThread {
	public : 
		inline explicit LockThread() {
			SingleThreaded::Lock();
		}
		inline ~LockThread() { 
			SingleThreaded::Unlock(); 
		}

	private : 
		inline explicit LockThread(LockThread const&) {}
		inline LockThread& operator=(LockThread const&) { return *this; }
	};    //    end of class LockThread

private : 
	friend LockThread;

	inline static void Lock() {}
	inline static void Unlock() {}

private : 
	inline explicit SingleThreaded(SingleThreaded const&) {}
	inline SingleThreaded& operator=(SingleThreaded const&) { return *this; }
};    //    end of class SingleThreaded


class GSLock
{

public:
	GSLock()
	{
		InitializeCriticalSection(&mSync);
	}

	~GSLock()
	{
		DeleteCriticalSection(&mSync);
	}

	inline void Enter()
	{
		EnterCriticalSection(&mSync);
	}

	inline void Leave()
	{
		LeaveCriticalSection(&mSync);
	}

private:
	CRITICAL_SECTION	mSync;
};


template<typename T>
class MultiThreaded {
public : 
	typedef T VolatileType;

protected : 
	inline explicit MultiThreaded() { }
	inline ~MultiThreaded() {  }

protected : 
	class LockThread {
	public : 
		inline explicit LockThread() {
			MultiThreaded::Lock();
		}
		inline ~LockThread() { 
			MultiThreaded::Unlock(); 
		}

	private : 
		inline explicit LockThread(LockThread const&) {}
		inline LockThread& operator=(LockThread const&) { return *this; }
	};    //    end of class LockThread

private : 
	friend LockThread;

	inline static void Lock()   { m_Lock.Enter(); }
	inline static void Unlock() { m_Lock.Leave(); }

private : 
	inline explicit MultiThreaded(MultiThreaded const&) {}
	inline MultiThreaded& operator=(MultiThreaded const&) { return *this; }

	static GSLock m_Lock;
};    //    end of class SingleThreaded

template<typename T>
GSLock MultiThreaded<T>::m_Lock;

template<typename T>
class DefaultLifetime {
protected : 
	inline explicit DefaultLifetime() {}
	inline ~DefaultLifetime() {}

	inline static void OnDeadReference() { 
		//	throw std::logic_error("Dead Reference Detected"); 
	}
	inline static void ScheduleForDestruction(void (*pFun)()) {
		atexit(pFun); 
	}

private : 
	inline explicit DefaultLifetime(DefaultLifetime const&) {}
	inline DefaultLifetime& operator=(DefaultLifetime const&) { return *this; }
};    //    end of class DefaultLifetime


template<typename T>
class CreateUsingNew {
protected : 
	inline explicit CreateUsingNew() {}
	inline ~CreateUsingNew() {}

	inline static T* CreateInstance() { return new T(); }
	inline static void DestroyInstance(T* t) { delete t; }

private : 
	inline explicit CreateUsingNew(CreateUsingNew const&) {}
	inline CreateUsingNew& operator=(CreateUsingNew const&) { return *this; }
}; 
//    end of class CreateUsingNew

template<typename T, typename CreationPolicy = CreateUsingNew<T>, 
	template <typename> class LifetimePolicy = DefaultLifetime, 
	template <typename> class ThreadingModel = MultiThreaded>
class Singleton : public CreationPolicy, public LifetimePolicy<T>, public ThreadingModel<T> {
public : 
	static T& Instance()
	{
		if ( Singleton::instance_ == 0 ) {
		//	LockThread lock;
			if ( Singleton::instance_ == 0 ) {
				if ( Singleton::destroyed_ ) {
					//OnDeadReference();
					Singleton::destroyed_ = false;
				}
				Singleton::instance_ = new T();// CreateInstance();
				try {
					ScheduleForDestruction(Singleton::Destroy);
				} catch(...) {
					DestroyInstance(Singleton::instance_);
				}        
			}
		}
		return *(Singleton::instance_);
	}
	static void Destroy()
	{
		if ( Singleton::instance_ != 0 ) {
		//	LockThread lock;
			if ( Singleton::instance_ != 0 ) {
				DestroyInstance(Singleton::instance_);
				Singleton::instance_ = 0;
				Singleton::destroyed_ = true;
			}
		}
	}

protected : 
	inline explicit Singleton() { 
		assert(Singleton::instance_ == 0); 
		Singleton::instance_ = static_cast<T*>(this); 
		Singleton::destroyed_ = false; 
	}
	inline ~Singleton() { 
		Singleton::instance_ = 0; 
		Singleton::destroyed_ = true; 
	}

private : 
	//static VolatileType* instance_;
	static T* instance_;
	static bool destroyed_;

private : 
	inline explicit Singleton(Singleton const&) {}
	inline Singleton& operator=(Singleton const&) { return *this; }
};    //    end of class Singleton


template<typename T, typename CreationPolicy,template <typename> class LifetimePolicy,template <typename> class ThreadingModel>
typename Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>:: VolatileType* Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::instance_ = 0;

template<typename T, typename CreationPolicy,template <typename> class LifetimePolicy,template <typename> class ThreadingModel>
bool Singleton<T, CreationPolicy, LifetimePolicy, ThreadingModel>::destroyed_ = false;

