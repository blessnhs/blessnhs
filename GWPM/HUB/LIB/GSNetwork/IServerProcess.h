#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "GSClient.h"

namespace GSNetwork {	namespace IServerProcess	{

class IServerProcess : public IProcess<GSCLIENT>
{
public:
	IServerProcess(void);
	~IServerProcess(void);

	VOID REGIST_SVR_REQ(LPVOID Data,DWORD Length);

	VOID REGIST_SERVER_RES(LPVOID Data,DWORD Length);
};

}	}