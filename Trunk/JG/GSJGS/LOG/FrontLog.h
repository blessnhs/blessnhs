#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class FrontLog : public GSLog<MultiThread> , public Singleton<FrontLog>
{
public:
	FrontLog(LPCTSTR file_name = _T("FrontLog.txt"));
	~FrontLog(void);
};

#define LOG Singleton<FrontLog>::Instance()
#define FLOG LOG.Write