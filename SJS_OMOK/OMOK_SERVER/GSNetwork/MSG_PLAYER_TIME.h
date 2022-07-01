#pragma once

#include "ICommand.h"
#include "./IMessage.h"
#include "GSServer.h"

namespace GSNetwork	{

class MSG_PLAYER_TIME :  public IMessage
{
public:
	MSG_PLAYER_TIME();
	virtual ~MSG_PLAYER_TIME();

	virtual void Execute(LPVOID Param);
	void Undo(){}

	GSServer::GSServer *pServer;
};

typedef boost::shared_ptr<MSG_PLAYER_TIME> MSG_PLAYER_TIME_PTR;

}