#pragma once

#include "GSServer.h"
#include "GSArgument.h"
#include "../Ini/INI.h"

class GSBoard : public GSNetwork::GSServer::GSServer
{
public:
	GSBoard(void);
	~GSBoard(void);

	BOOL Initialize();

	BOOL Disconnect(GSCLIENT_PTR object);
	VOID Accept(GSCLIENT_PTR object);

	BoardINI			  m_Ini;

	BOOL DeleteAllConcurrentUser();
	
};

#define SERVER Singleton<GSBoard>::Instance()