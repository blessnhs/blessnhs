#pragma once

#include "../IOCP/TCP/TCPSession.h"
#include "ICommand.h"
#include "GSPacketTCP.h"

#include "MSG.h"

class MSG_CLOSE : public SvrMSG
{
public:
	MSG_CLOSE() {}
	~MSG_CLOSE(){}

	TCP_SESSION		*pSession;
	TCP_SESSION		*pListen;

	void Execute(LPVOID Param);
	
	void Undo(){}
};

typedef boost::shared_ptr<MSG_CLOSE> MSG_CLOSE_PTR;