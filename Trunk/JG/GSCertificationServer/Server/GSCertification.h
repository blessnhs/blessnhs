#pragma once

#include "GSServer.h"
#include "GSArgument.h"
#include "../Ini/CertificationINI.h"

class GSCertification : public GSNetwork::GSServer::GSServer
{
public:
	GSCertification(void);
	~GSCertification(void);

	BOOL Initialize();

	BOOL Disconnect(GSCLIENT *object);
	VOID Accept(GSCLIENT *object);

	CertificationINI			  m_Ini;
	
};

