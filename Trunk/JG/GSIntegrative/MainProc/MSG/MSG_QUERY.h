#pragma  once

#include "../NetworkSession/NetworkSession.h"
#include "ICommand.h"
#include "MSG.h"

class MSG_QUERY : public SvrMSG
{
public:
	MSG_QUERY();
	~MSG_QUERY();

	IResponse		*pSession;

	DWORD MsgId;

	LPVOID pRequst;

	void Execute(LPVOID Param);
	void Undo(){}
};
