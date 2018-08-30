#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "../NetworkSession.h"

namespace GSFrontServer {	namespace IFieldServerProcess	{

class IFieldServerProcess : public IProcess<SESSION>
{
public:
	IFieldServerProcess(void);
	~IFieldServerProcess(void);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

	VOID REGIST_FIELDSERVER(LPVOID Data,DWORD Length);
	VOID CLOSE_PLAYER(LPVOID Data,DWORD Length);
	VOID REGISTER_PLAYER(LPVOID Data,DWORD Length);

protected:
	
	DECLARE_MSG_MAP(IFieldServerProcess)
};

}	}