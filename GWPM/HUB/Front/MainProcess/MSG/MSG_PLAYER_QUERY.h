#pragma once

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace Front	{

	class GSAllocator;

	template<class T>
	class MSG_PLAYER_QUERY ;


#define DECLARE_DB_CLASS_BEGIN(class_name) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
virtual ~class_name() {};\
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_1(class_name,type1) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_2(class_name,type1,type2) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
std::tuple<type1,type2> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\


#define DECLARE_DB_CLASS_BEGIN_3(class_name,type1,type2,type3) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_4(class_name,type1,type2,type3,type4) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\

#define DECLARE_DB_CLASS_BEGIN_5(class_name,type1,type2,type3,type4,type5) \
class class_name:public GSObject \
{ \
public:\
class_name() {}\
    std::tuple<type1,type2,type3,type4,type5> m_args; \
};	\
template<>\
class MSG_PLAYER_QUERY<##class_name>:public IMESSAGE	\
{\
	public:	\
		MSG_PLAYER_QUERY() {}\
		~MSG_PLAYER_QUERY() {}\
	GSCLIENT_PTR pSession; \
	class_name Request;\
	void Execute(LPVOID Param)\
	{\
		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type); \
		if (pProcess == NULL || pProcess->m_IsOpen == false)\
		{\
			return; \
		}\


#define DECLARE_DB_CLASS_END	\
	}\
		void Undo() {}\
}; 

	template<class T>
	class MSG_PLAYER_QUERY : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY() = default;
		~MSG_PLAYER_QUERY() = default;

		DBPROCESS_CER_PTR   pProcess;

		boost::shared_ptr<T> pRequst;

		void Execute(LPVOID Param)
		{
			printf("error MSG_PLAYER_QUERY Type Not Define \n");
		}


		void Undo() {}
	};

	//
	//#pragma region RequestDeleteAllConcurrentUser
	//	DECLARE_DB_CLASS_BEGIN(RequestDeleteAllConcurrentUser)
	//	pProcess->DeleteAllConcurrentUser();
	//	DECLARE_DB_CLASS_END
	//#pragma endregion
	//
	//
	//#pragma region RequestVersion
	//	DECLARE_DB_CLASS_BEGIN(RequestVersion)
	//	{
	//		VERSION_RES res;
	//
	//		if (pSession == NULL || pSession->GetConnected() == false)
	//		{
	//			res.set_var_code(DataBaseError);
	//
	//			SEND_PROTO_BUFFER(res, pSession)
	//				return;
	//		}
	//
	//		DBPROCESS_CER_PTR pProcess = DBPROCESSCONTAINER_CER.Search(Type);
	//		if (pProcess == NULL || pProcess->m_IsOpen == false)
	//		{
	//			res.set_var_code(DataBaseError);
	//
	//			SEND_PROTO_BUFFER(res, pSession)
	//				return;
	//		}
	//
	//		// 로그인 절차 : 아이디의 접속확인 및 인증키값을 가져온다.
	//		float nRet = pProcess->ProcedureVersion();
	//
	//		res.set_var_version(nRet);
	//		res.set_var_code(Success);
	//
	//		SEND_PROTO_BUFFER(res, pSession)
	//	}
	//	DECLARE_DB_CLASS_END
	//#pragma endregion

	}


#include "MSG_PLAYER_QUERY.hpp"