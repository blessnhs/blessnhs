#include "StdAfx.h"
#include "Adapt.h"
#include "IPlayerProcess.h"
#include "IServerProcess.h"

namespace GSNetwork {	namespace Adapt	{

Adapt::Adapt(void)
{
}


Adapt::~Adapt(void)
{
}

IProcess<GSCLIENT> *Adapt::Create(E_TYPE nType)
{
/*	switch(nType)
	{
	case PLAYER:
		{
			return new GSNetwork::IPlayerProcess::IPlayerProcess();
		}
		break;
	case _SERVER_1:
		{
			return new GSNetwork::IServerProcess::IServerProcess();
		}
		break;
	}
	*/
	return NULL;
}

}	}
