#pragma once

#include "IPlayerProcess.h"

class CertificationProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	CertificationProcess(void);
	~CertificationProcess(void);

	VOID LOGIN_PLAYER(LPVOID Data,DWORD Length);

	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

protected:

	DECLARE_MSG_MAP(CertificationProcess)

};

