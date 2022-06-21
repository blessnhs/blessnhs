#include "StdAfx.h"
#include "GSArgument.h"


GSArgument::GSArgument(void)
{
	m_MaxClient = 1000;
	m_BindPort  = 8245;
	m_LogicThreadCnt = 8;
	m_Version = 0;
	m_Naggle = false;
	m_AliveTime = 10000;
}


GSArgument::~GSArgument(void)
{
}
