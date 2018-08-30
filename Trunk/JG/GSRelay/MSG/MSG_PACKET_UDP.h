#pragma once
#include "../IOCP/TCP/TCPSession.h"
#include "../IOCP/UDP/UDPSession.h"
#include "ICommand.h"
#include "../Process/Adapt.h"
#include "MSG.h"

class MSG_PACKET_UDP : public SvrMSG
{
public:
	MSG_PACKET_UDP();
	~MSG_PACKET_UDP(){}

	UDP_SESSION		*pSession;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PACKET_UDP> MSG_PACKET_UDP_PTR;