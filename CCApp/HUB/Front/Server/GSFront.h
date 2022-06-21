#pragma once

#include "GSServer.h"
#include "GSArgument.h"
#include "../Ini/INI.h"

class GSFront : public GSNetwork::GSServer::GSServer
{
public:
	GSFront(void);
	~GSFront(void);

	BOOL Initialize();

	BOOL Disconnect(GSCLIENT_PTR object);
	VOID Accept(GSCLIENT_PTR object);

	FrontINI			  m_Ini;
	
};

#define SERVER Singleton<GSFront>::Instance()