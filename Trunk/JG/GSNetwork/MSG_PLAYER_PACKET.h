#pragma once

#include "GSClient.h"
#include "./IMessage.h"
#include "GSClient.h"

namespace GSNetwork	{

class MSG_PLAYER_PACKET : public IMessage
{
public:
	MSG_PLAYER_PACKET();
	~MSG_PLAYER_PACKET();

	GSCLIENT		*pClient;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PLAYER_PACKET> MSG_PLAYER_PACKET_PTR;

}