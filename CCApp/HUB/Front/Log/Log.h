#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class FrontLog : public GSLog , public Singleton<FrontLog>
{
public:
	FrontLog(char *filename = "FrontLog.txt");
	~FrontLog(void);
};

#define LOG Singleton<FrontLog>::Instance()
#define BLOG LOG.Write