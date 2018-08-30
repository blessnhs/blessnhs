#include "StdAfx.h"

#include "CertificationProcess.h"
#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "Protocol.h"
#include "GSSerialize.h"

#include "../DB/DBProxy/DBProcess.h"
#include "../DB/DBProxy/DBProcessContainer.h"
#include "../DB/DBJob/RequestPlayerAuth.h"

#include "GSAllocator.h"
#include "GSMainProcess.h"

#include "MessageMapDef.h"

#include "Auth.h"

#include "../MainProcess/MSG/MSG_PLAYER_QUERY.h"


CertificationProcess::CertificationProcess(void)
{
	ADD_NET_FUNC(CertificationProcess,ID_AUTH_LOGIN_REQ,LOGIN_PLAYER);
}


CertificationProcess::~CertificationProcess(void)
{
}

VOID CertificationProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}


VOID CertificationProcess::LOGIN_PLAYER(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_WSTR_MEMBER("Id",Id)
	GET_JSON_WSTR_MEMBER("Passwd",Passwd)

	WORD nRet = _ERR_NONE;
	
	DBPROCESS_CER_PTR pProc = DBPROCESSCONTAINER_CER.Search(pOwner->GetMyDBTP());
	if(pProc == NULL) 
		return ;

	if(pProc->m_pDB->IsOpen()==false) 
	{
		return ;
	}

	pOwner->IncDBStamp();
	//로그인 쿼리를 날린다.
	boost::shared_ptr<RequestPlayerAuth> pRequest = ALLOCATOR.Create<RequestPlayerAuth>();
	pRequest->Account     = Id;
	pRequest->Passwd      = Passwd;
	PROC_REG_QUERY_JOB(pRequest,DBP_GET_LOGIN_INFO,pOwner,Certification::MSG_PLAYER_QUERY,RequestPlayerAuth)

	return ;

}