#pragma once
#include "TypeList.h"

#ifdef REMOVE_BOOST
#else
#include <boost/shared_ptr.hpp>
#endif

template<typename R,class TList>
class FunctorImpl;

template<typename R>
class FunctorImpl<R,Empty>
{
public:
	virtual R operator() () = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1>
class FunctorImpl<R,TYPELIST_1(P1)>
{
public:
	virtual R operator() (P1) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1,typename P2>
class FunctorImpl<R,TYPELIST_2(P1,P2)>
{
public:
	virtual R operator() (P1,P2) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1,typename P2,typename P3>
class FunctorImpl<R,TYPELIST_3(P1,P2,P3)>
{
public:
	virtual R operator() (P1,P2,P3) = 0;
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


	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2,typename ParentFunctor::Param3 p3)
	{
		return _fun(p1,p2,P3);
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

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2,typename ParentFunctor::Param3 p3)
	{
		return ((*pObj_).*pMemFn_)(p1,p2,p3);
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
	typedef typename TypeAt<TList,0>::Result  Param1;
	typedef typename TypeAt<TList,1>::Result  Param2;
	typedef typename TypeAt<TList,2>::Result  Param3;
	typedef R ResultType;

public:
	Functor(){}
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

	R operator()(Param1 p1,Param2 p2,Param2 p3)
	{
		return (*spImpl)(p1,p2,p3);
	}

public:
#ifdef REMOVE_BOOST
	std::shared_ptr<Impl> spImpl;
#else
	boost::shared_ptr<Impl> spImpl;
#endif
	
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