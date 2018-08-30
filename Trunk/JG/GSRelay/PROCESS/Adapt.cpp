#include "StdAfx.h"
#include "Adapt.h"
#include "IRelayProcess.h"
#include "IUdpProcess.h"
#include "ITcpProcess.h"

Adapt::Adapt(void)
{
}


Adapt::~Adapt(void)
{
}

IProcess<TCP_SESSION> *Adapt::Create(E_TYPE nType)
{
	switch(nType)
	{
	case RELAY_SERVER:
		return new IRelayProcess();
		break;
	case TCP_CLIENT:
		return new ITCPProcess();
		break;;

	}
	return NULL;
}
