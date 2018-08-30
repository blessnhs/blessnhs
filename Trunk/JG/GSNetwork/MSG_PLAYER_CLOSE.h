#pragma once
#include "ICommand.h"
#include "GSPacketTCP.h"
#include "IProcess.h"
#include "GSClient.h"
#include "./IMessage.h"

typedef GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;

namespace GSNetwork	{

class MSG_PLAYER_CLOSE :  public IMessage 
{
public:
	MSG_PLAYER_CLOSE();
	~MSG_PLAYER_CLOSE();

	GSCLIENT			*pClient;
	GSCLIENT			*pListen;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PLAYER_CLOSE> MSG_PLAYER_CLOSE_PTR;
}