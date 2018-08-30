#include "StdAfx.h"
#include "GSMainProcess.h"

namespace GSNetwork	{	namespace GSMainProcess	{

GSMainProcess *g_MainProc = NULL;

GSMainProcess::GSMainProcess(GSProactorImpl *ProactorImpl)
:GSProactor(ProactorImpl)
{
}

GSMainProcess::GSMainProcess()
	:GSProactor(new GSProactorImpl())
{

}


GSMainProcess::~GSMainProcess(void)
{
}

void GSMainProcess::OnEvt(LPVOID Arg)
{
}

}	}