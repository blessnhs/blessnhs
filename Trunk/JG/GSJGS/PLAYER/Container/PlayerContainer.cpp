#include "StdAfx.h"
#include "PlayerContainer.h"

namespace GSFrontServer	{	 namespace PlayerContainer	{

extern PlayerContainer<GSCreator> &GetPlayerContainer()
{
	static PlayerContainer<GSCreator> g_PlayerContainer;
	return g_PlayerContainer;
}

}	}