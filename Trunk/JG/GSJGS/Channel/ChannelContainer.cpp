	#include "StdAfx.h"
#include "ChannelContainer.h"

namespace GSFrontServer	{	namespace	ChannelContainer	{

extern ChannelContainer<GSCreator> &GetChannelContainer()
{
	static ChannelContainer<GSCreator> g_ChannelContainer;
	return g_ChannelContainer;
}


}	}