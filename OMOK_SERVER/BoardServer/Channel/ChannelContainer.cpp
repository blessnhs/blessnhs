#include "StdAfx.h"
#include "ChannelContainer.h"

extern ChannelContainer<GSCreator> & GetChannelContainer()
{
	static ChannelContainer<GSCreator> g_ChannelContainer;
	return g_ChannelContainer;
}

