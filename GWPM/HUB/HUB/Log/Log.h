#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class HubLog : public GSLog , public Singleton<HubLog>
{
public:
	HubLog(char *filename = "HubLog.txt");
	~HubLog(void);
};

#define LOG Singleton<HubLog>::Instance()
#define BLOG LOG.Write