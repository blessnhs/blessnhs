#pragma once

#include "GSServer.h"
#include "GSArgument.h"

class GSIntergrativeServer : public GSNetwork::GSServer::GSServer
{
public:
	GSIntergrativeServer(void);
	~GSIntergrativeServer(void);

	BOOL Initialize();

	BOOL Disconnect(GSCLIENT *object);
	VOID Accept(GSCLIENT *object);
};

