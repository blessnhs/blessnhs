#pragma once

#include "ICommand.h"
#include "GSClient.h"
#include "./IMessage.h"

namespace GSNetwork	{

class MSG_PLAYER_ACCEPT : public IMessage
{
public:
	MSG_PLAYER_ACCEPT();
	virtual ~MSG_PLAYER_ACCEPT();

	GSCLIENT_PTR		pClient;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PLAYER_ACCEPT> MSG_PLAYER_ACCEPT_PTR;

}