#include "StdAfx.h"
#include "Adapt.h"
#include "IPlayerProcess.h"
#include "IFieldServerProcess.h"

namespace GSFrontServer {	namespace Adapt	{

Adapt::Adapt(void)
{
}


Adapt::~Adapt(void)
{
}

IProcess<SESSION> *Adapt::Create(E_TYPE nType)
{
	switch(nType)
	{
	case PLAYER:
		{
			return new GSFrontServer::IPlayerProcess::IPlayerProcess();
		}
		break;
	case FIELD_SERVER:
		{
			return new GSFrontServer::IFieldServerProcess::IFieldServerProcess();
		}
		break;
	}

	return NULL;
}

}	}
