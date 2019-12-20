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

	BOOL Disconnect(GSCLIENT *object);
	VOID Accept(GSCLIENT *object);

	BoardINI			  m_Ini;
	
};

#define SERVER Singleton<GSBoard>::Instance()