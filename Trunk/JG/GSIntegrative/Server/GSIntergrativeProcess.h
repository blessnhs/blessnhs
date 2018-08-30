#pragma once

#include "IServerProcess.h"

class GSIntergrativeProcess : public GSNetwork::IServerProcess::IServerProcess
{
public:
	GSIntergrativeProcess(void);
	~GSIntergrativeProcess(void);

	VOID ACCESS_GAMESVR(LPVOID Data,DWORD Length);
	VOID REGIST_FRONT_SVR(LPVOID Data,DWORD Length);
	VOID REGIST_PLAYER(LPVOID Data,DWORD Length);
	VOID CLOSE_PLAYER(LPVOID Data,DWORD Length);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

protected:

	DECLARE_MSG_MAP(GSIntergrativeProcess)
};

