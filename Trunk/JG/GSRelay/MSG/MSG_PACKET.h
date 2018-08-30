#pragma once
#include "../IOCP/TCP/TCPSession.h"
#include "../IOCP/UDP/UDPSession.h"
#include "ICommand.h"
#include "../Process/Adapt.h"
#include "MSG.h"

class MSG_PACKET : public SvrMSG
{
public:
	MSG_PACKET();
	~MSG_PACKET(){}

	WORD MainProtocol;
	WORD SubProtocol;
	WORD dwPacketLength;
	TCP_SESSION		*pSession;
	UDP_SESSION		*pSession_UDP;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PACKET> MSG_PACKET_PTR;