#pragma once

#include "../NetworkSession/NetworkSession.h"
#include "ICommand.h"
#include "GSPacketTCP.h"

#include "MSG.h"

class MSG_CLOSE : public SvrMSG
{
public:
	MSG_CLOSE() {}
	~MSG_CLOSE(){}

	SESSION		*pSession;
	GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP			*pListen;

	void Execute(LPVOID Param);
	
	void Undo(){}
};

typedef boost::shared_ptr<MSG_CLOSE> MSG_CLOSE_PTR;