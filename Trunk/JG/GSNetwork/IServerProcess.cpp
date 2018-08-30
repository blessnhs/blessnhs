#include "StdAfx.h"
#include "IServerProcess.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "../GSPacket/protocol.h"
#include "../GSPacket/Front.h"

#include "GSSerialize.h"

namespace GSNetwork {	namespace IServerProcess	{

IServerProcess::IServerProcess(void)
{
}


IServerProcess::~IServerProcess(void)
{
}

VOID IServerProcess::REGIST_SVR_REQ(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Id",Id)

	printf("FontServer Regist Success \n");

	pOwner->SetType(FRONT_SERVER);

	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Result",0)
	END_JSON_MEMBER(pOwner->GetTCPSocket(),ID_FG_PKT_REGIST_RES)

}

VOID IServerProcess::REGIST_SERVER_RES(LPVOID Data,DWORD Length)
{
	
	DECLARE_JSON_READ
	GET_JSON_INT_MEMBER("Result",Result)

	GSCLIENT *pSession = (GSCLIENT *)this->pOwner;

	pSession->SetType(_SERVER_);

	printf("Accept Field Server Regist Result = %d\n",Result);
}

}	}