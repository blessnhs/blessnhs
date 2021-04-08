#include "StdAfx.h"
#include "PlayerContainer.h"

extern PlayerContainer<GSCreator> &GetPlayerContainer()
{
	static PlayerContainer<GSCreator> g_PlayerContainer;
	return g_PlayerContainer;
}

