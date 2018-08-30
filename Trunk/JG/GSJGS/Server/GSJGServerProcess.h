#pragma once

#include "IPlayerProcess.h"

class GSJGServerProcess  : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	GSJGServerProcess(void);
	~GSJGServerProcess(void);

	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

	VOID REGIST_FIELDSERVER(LPVOID Data,DWORD Length);
	VOID CLOSE_PLAYER(LPVOID Data,DWORD Length);
	VOID REGISTER_PLAYER(LPVOID Data,DWORD Length);

protected:

	DECLARE_MSG_MAP(GSJGServerProcess)
};

