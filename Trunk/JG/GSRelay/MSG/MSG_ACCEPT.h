#pragma once

#include "../IOCP/TCP/TCPSession.h"
#include "MSG.h"

class MSG_ACCEPT : public SvrMSG
{
public:
	MSG_ACCEPT() {}
	~MSG_ACCEPT(){}

	TCP_SESSION		*pSession;

	void Execute(LPVOID Param);
	
	void Undo(){}
};

typedef boost::shared_ptr<MSG_ACCEPT> MSG_ACCEPT_PTR;