#pragma once

#include "IProcess.h"
#include "../IOCP/TCP/TCPSession.h"

class Adapt
{
public:
	Adapt(void);
	~Adapt(void);

	enum E_TYPE
	{
		DBA	=	0,
		FIELD_SERVER,
		FRONT_SERVER,
		RELAY_SERVER,
		UDP_CLIENT,
		TCP_CLIENT,
		
	};

	static IProcess<TCP_SESSION> *Create(E_TYPE nType);

};

