#pragma once

#include "ICommand.h"
#include "GSClient.h"
#include "./IMessage.h"

class MSG_PACKET_UDP : public IMESSAGE
{
public:
	MSG_PACKET_UDP();
	virtual ~MSG_PACKET_UDP(){}

	GSCLIENT_PTR		pClient;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PACKET_UDP> MSG_PACKET_UDP_PTR;