#include "StdAfx.h"
#include "GSMainProcess.h"

namespace GSRelayServer	{	namespace GSMainProcess	{

GSMainProcess *g_MainProc = NULL;

GSMainProcess::GSMainProcess(GSProactorImpl *ProactorImpl)
:GSProactor(ProactorImpl)
{
}


GSMainProcess::~GSMainProcess(void)
{
}

void GSMainProcess::OnEvt(LPVOID Arg)
{
}

GSMainProcess *GetMainProc()
{
	if(g_MainProc == NULL)
		g_MainProc = new GSMainProcess(new GSProactorImpl());

	return g_MainProc;
}
}	}