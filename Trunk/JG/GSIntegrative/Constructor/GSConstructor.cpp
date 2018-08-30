#include "StdAfx.h"
#include "GSConstructor.h"
#include "../PLAYER/PlayerManager.h"
#include "../Server/GSIntergrativeServer.h"

GSConstructor::GSConstructor(void)
{
	GSIntergrativeServer *_instance = new GSIntergrativeServer;
}


GSConstructor::~GSConstructor(void)
{
}

