#pragma once

template<typename T>
struct Type2Type
{
	typedef T OriginalType;
};

template<typename T>
struct Int2Type
{
	enum { value = v };
};

template<class T,class U>
struct TypeList
{
	typedef T Head;
	typedef U Tail;
};

struct Empty {};
struct NullType {};

#define TYPELIST_1(T1) TypeList<T1,Empty>
#define TYPELIST_2(T1,T2) TypeList<T1,TYPELIST_1(T2)>
#define TYPELIST_3(T1,T2,T3) TypeList<T1,TYPELIST_2(T2,T3)>
#define TYPELIST_4(T1,T2,T3,T4) TypeList<T1,TYPELIST_3(T2,T3,T4)>
#define TYPELIST_5(T1,T2,T3,T4,T5) TypeList<T1,TYPELIST_4(T2,T3,T4,T5)>
#define TYPELIST_6(T1,T2,T3,T4,T5,T6) TypeList<T1,TYPELIST_5(T2,T3,T4,T5,T6)>

template<class TList> struct Length;;
template<> struct Length<Empty>
{
	enum { value = 0};
};
template<class T,class U>
struct Length<TypeList<T,U>>
{
	enum { value = 1 + Length<U>::value };
};


template<class TList,unsigned int i> struct TypeAt;

template<unsigned int i>
struct TypeAt<Empty,i>
{
	typedef Empty Result;
};

template<class T,class U>
struct TypeAt<TypeList<T,U>,0>
{
	typedef T Result;
};

template<class Head,class Tail,unsigned int i>
struct TypeAt<TypeList<Head,Tail>,i>
{
	typedef typename TypeAt<Tail,i-1>::Result Result;
};

template<class TList,class T> struct IndexOf;

template<class T>
struct IndexOf<Empty,T>
{
	enum { value = -1 };
};

template<class T,class Tail>
struct IndexOf<TypeList<T,Tail>,T>
{
	enum { value = 0 };
};

template<class Head,class Tail,class T>
struct IndexOf<TypeList<Head,Tail>,T>
{
private:
	enum { temp = IndexOf<Tail,T>::value };
public:
	enum { value = temp == -1 ? -1 : 1 + temp  };
};
template <class TList,class T> struct Append;

template<> struct Append<Empty,Empty>
{
	typedef Empty Result;
};


template<class T> struct Append<Empty,T>
{
	typedef TYPELIST_1(T) Result;
};

template<class Head,class Tail>
struct Append<Empty,TypeList<Head,Tail>>
{
	typedef TypeList<Head,Tail> Result;
};

template<class Head,class Tail,class T>
struct Append<TypeList<Head,Tail>,T>
{
	typedef TypeList<Head,typename Append<Tail,T>::Result>
		Result;
};

template<class TList,class T>struct Erase;

template<class T>
struct Erase<Empty,T>
{
	typedef Empty Result;
};

template<class T,class Tail>
struct Erase<TypeList<T,Tail>,T>
{
	typedef Tail Result;
};

template<class Head,class Tail,class T>
struct Erase<TypeList<Head,Tail>,T>
{
	typedef TypeList<Head,typename Erase<Tail,T>::Result>
		Result;
};

template<class TList,class T> struct EraseAll;

template<class T>
struct EraseAll<Empty,T>
{
	typedef Empty Result;
};

template<class T,class Tail>
struct EraseAll<TypeList<T,Tail>,T>
{
	typedef typename EraseAll<Tail,T>::Result Result;
};

template<class Head,class Tail,class T>
struct EraseAll<TypeList<Head,Tail>,T>
{
	typedef TypeList<Head,typename EraseAll<Tail,T>::Result> Result;
};

