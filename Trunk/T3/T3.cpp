// T3.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <assert.h>

struct NullType;

template<class T,class U>
struct TypeList
{
	typedef T Head;
	typedef U Tail;
};

template<class TList> struct Length;
template<> struct Length<NullType>
{
	enum {value = 0 };
};

template<class T,class U>
struct Length<TypeList<T,U>>
{
	enum {value = 1 + Length<U>::value };
};

template<class TList,int index> struct TypeAt;

template<class Head,class Tail>
struct TypeAt<TypeList<Head,Tail>, 0>
{
	typedef Head Result;
};

template<int index>
struct TypeAt<NullType,index>
{
	typedef int Result;
};

template<class Head,class Tail,int index>
struct TypeAt<TypeList<Head,Tail>, index>
{
	typedef typename TypeAt<Tail,index - 1>::Result Result;
	
};		

template<class TList,class T> struct IndexOf;

template<class T>
struct IndexOf<NullType, T>
{
	enum { value = -1 };
};

template<class T,class Tail>
struct IndexOf<TypeList<T,Tail>, T>
{
	enum { value = 0 };
};	

template<class Head,class Tail,class T>
struct IndexOf<TypeList<Head,Tail>, T>
{
private:

	enum { temp = IndexOf<Tail,T>::value };
public:
	enum { value = temp == -1 ? -1 : 1 + temp };
};

typedef TypeList<int,TypeList<float,TypeList<double,NullType>> > list;


struct Chunk
{
	void Init(size_t blockSize,unsigned char blocks);
	void *Allocate(size_t blockSize);
	void Deallocate(void *p,size_t blockSize);

	unsigned char *pData;
	unsigned char firstAvailableBlock_,
		blockAvailable;
};

void Chunk::Init(size_t blockSize,unsigned char blocks)
{
	pData = new unsigned char[blockSize * blocks];
	firstAvailableBlock_ = 0;
	blockAvailable       = blocks;
	unsigned char i = 0;
	unsigned char*p = pData;

	for(;i != blocks; p += blockSize)
	{
		*p = ++i;
			
	}
}

void *Chunk::Allocate(size_t blockSize)
{
	if(!blockAvailable) return 0;
	unsigned char *pResult = pData + (firstAvailableBlock_ * blockSize);

	firstAvailableBlock_ = *pResult;
	--blockAvailable;
	return pResult;
}

void Chunk::Deallocate(void *p,size_t blockSize)
{
	assert(p >= pData);

	unsigned char *toRelease = static_cast<unsigned char *>(p);

	assert((toRelease - pData) % blockSize == 0);

	*toRelease = firstAvailableBlock_;

	firstAvailableBlock_ = static_cast<unsigned char>((toRelease - pData) / blockSize);

	assert(firstAvailableBlock_ == (toRelease - pData) / blockSize);

	++blockAvailable;
}

template<typename R,class TList>
class FunctorImpl;

template<typename R>
class FunctorImpl<R,NullType>
{
	public:
		virtual R operator() () = 0;
		virtual FunctorImpl* Clone() const = 0;
		virtual ~FunctorImpl() {}
};

template<typename R,typename P1>
class FunctorImpl<R,TypeList<P1,NullType>>
{
public:
	virtual R operator() (P1) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1,typename P2>
class FunctorImpl<R,TypeList<P1,TypeList<P2,NullType>>>
{
public:
	virtual R operator() (P1,P2) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<class ParentFunctor,typename Fun>
class FunctorHandler : public FunctorImpl
	<
	typename ParentFunctor::ResultType,
	typename ParentFunctor::ParmList
	>
{
public:
	typedef typename  ParentFunctor::ResultType ResultType;

	FunctorHandler(const Fun &fun) : _fun(fun) {}
	FunctorHandler* Clone() const { return new FunctorHandler(*this); }

	ResultType operator()()
	{
		return _fun();
	}

	ResultType operator()(typename ParentFunctor::Param1 p1)
	{
		return _fun(p1);
	}

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2)
	{
		return _fun(p1,p2);
	}
public:
	Fun _fun;
};

template<class ParentFunctor,typename PointerToObj,typename PointerToMemFn>
class MemFunHandler : public FunctorImpl
	<
	typename ParentFunctor::ResultType,
	typename ParentFunctor::ParmList
	>
{
public:
	typedef typename  ParentFunctor::ResultType ResultType;

	MemFunHandler(const PointerToObj &pObj,PointerToMemFn pMemFn) : pObj_(pObj),pMemFn_(pMemFn) {}
	MemFunHandler* Clone() const { return new MemFunHandler(*this); }

	ResultType operator()()
	{
		return ((*pObj_).*pMemFn_)();
	}

	ResultType operator()(typename ParentFunctor::Param1 p1)
	{
		return  ((*pObj_).*pMemFn_)(p1);
	}

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2)
	{
		return ((*pObj_).*pMemFn_)(p1,p2);
	}
public:
	PointerToObj	pObj_;
	PointerToMemFn	pMemFn_;
};

template<typename R,class TList>
class Functor
{
public:
	typedef FunctorImpl<R,TList> Impl;

	typedef TList ParmList;
	typedef typename TypeAt<TList,0>::Result Param1;
	typedef typename TypeAt<TList,1>::Result Param2;
	typedef R ResultType;

public:
	Functor();
	Functor(const Functor&);
	Functor& operator-=(const Functor &);
	explicit Functor(Impl spImpl);

	template<typename Fun,typename ClassType>
	Functor(const Fun fun,const ClassType &classType);

	template<typename Fun>
	Functor(const Fun fun);

	R operator()()
	{
		return (*spImpl)();
	}

	R operator()(Param1 p1)
	{
		return (*spImpl)(p1);
	}

	R operator()(Param1 p1,Param2 p2)
	{
		return (*spImpl)(p1,p2);
	}

public:
	Impl *spImpl;
};


template<typename R,class TList>
template<typename Fun,typename ClassType>
Functor<R,TList>::Functor(const Fun fun,const ClassType &classType)
	:spImpl(new MemFunHandler<Functor,ClassType,Fun>(classType,fun))
{

}

template<typename R,class TList>
template<typename Fun>
Functor<R,TList>::Functor(const Fun fun)
	:spImpl(new FunctorHandler<Functor,Fun>(fun))
{

}


struct testfunctor
{
	void operator()(int a)
	{
		printf("hello world\n");
	}
};


class ss
{
public:
	ss(){}
	~ss(){}

	void print(int a) {printf("call\n"); }
};


int _tmain(int argc, _TCHAR* argv[])
{
	testfunctor tt;

	ss _ss;


	Functor<void,TypeList<int,NullType>> func(&ss::print,&_ss);

	func(0);

	Functor<void,TypeList<int,NullType>> func2(tt);

	func2(0);


//	Functor<> func2(_ss,ss::print);

	typedef void (ss::*TpMemFn)(int);

	TpMemFn pActivate = &ss::print;

	ss geronimo;

	ss* pss = &geronimo;

	(geronimo.*pActivate)(0);

//	(geronimo->*pActivate)(0);

	pActivate = &ss::print;

	(geronimo.*pActivate)(1);

	return 0;
}

