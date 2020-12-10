#include "StdAfx.h"
#include "IServerProcess.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

namespace GSNetwork {	namespace IServerProcess	{

IServerProcess::IServerProcess(void)
{
}


IServerProcess::~IServerProcess(void)
{
}

VOID IServerProcess::REGIST_SVR_REQ(LPVOID Data,DWORD Length)
{
	/*DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Id",Id)

	SYSLOG().Write("FontServer Regist Success \n");


	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result",0)
	END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FG_PKT_REGIST_RES)
*/
}

VOID IServerProcess::REGIST_SERVER_RES(LPVOID Data,DWORD Length)
{
	
	/*DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Result",Result)

	GSCLIENT *pSession = (GSCLIENT *)this->pOwner;

	pSession->SetType(_SERVER_);

	SYSLOG().Write("Accept Field Server Regist Result = %d\n",Result);*/
}

}	}