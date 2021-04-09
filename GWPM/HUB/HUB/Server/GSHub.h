#pragma once

#include "GSServer.h"
#include "GSArgument.h"
#include "../Ini/INI.h"

class GSHub : public GSNetwork::GSServer::GSServer
{
public:
	GSHub(void);
	~GSHub(void);

	BOOL Initialize();

	BOOL Disconnect(GSCLIENT_PTR object);
	VOID Accept(GSCLIENT_PTR object);

	HubINI			  m_Ini;

	BOOL DeleteAllConcurrentUser();
	
};

#define SERVER Singleton<GSHub>::Instance()