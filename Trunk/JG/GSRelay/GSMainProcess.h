#pragma once
#include "GSProactor.h"

namespace GSRelayServer	{	namespace GSMainProcess	{

typedef GSFrames::GSProactorImpl::GSProactorImpl GSProactorImpl;

class GSMainProcess : public GSFrames::GSProactor::GSProactor
{
public:
	enum EVT_TYPE
	{
		MSG_TYPE_USER=0,
		MSG_TYPE_UDP_1,
		MSG_TYPE_LOG,
	};

	GSMainProcess(GSProactorImpl *ProactorImpl);
	~GSMainProcess(void);

	virtual void OnEvt(LPVOID Arg);
};

extern GSMainProcess *GetMainProc();

}	}

#define  MAINPROC GSRelayServer::GSMainProcess::GetMainProc()