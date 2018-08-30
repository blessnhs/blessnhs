#pragma once

#include "GSServer.h"

#include "SingleTonHolder.h"

class GSJGServer  : public GSNetwork::GSServer::GSServer , public Singleton<GSJGServer>
{
public:
	GSJGServer();
	~GSJGServer();


	BOOL Initialize();

	BOOL Disconnect(GSCLIENT *object);
	VOID Accept(GSCLIENT *object);

};

#define JGSERVER Singleton<GSJGServer>::Instance()