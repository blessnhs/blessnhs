#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class BoardLog : public GSLog , public Singleton<BoardLog>
{
public:
	BoardLog(char *filename);
	~BoardLog(void);
};

#define LOG Singleton<BoardLog>::Instance()
#define BLOG LOG.Write