#pragma once

#include "GSLog.h"
#include "SingleTonHolder.h"

class BoardLog : public GSLog<MultiThread> , public Singleton<BoardLog>
{
public:
	BoardLog(LPCTSTR file_name = _T("BoardLogLog.txt"));
	~BoardLog(void);
};

#define LOG Singleton<BoardLog>::Instance()
#define BLOG LOG.Write