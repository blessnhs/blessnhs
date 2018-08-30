#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class Log : public GSLog<MultiThread> , public Singleton<Log>
{
public:
	Log(LPCTSTR file_name = _T("GSIntergrativeLog.txt"));
	~Log(void);
};

#define LOG Singleton<Log>::Instance()
#define GLOG LOG.Write