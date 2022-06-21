//! @date	2015/04/02
//!	@file 	CrashRepoter.h
//!	@author	gomsilruk
//!	@brief

#include <windows.h>
#include <string.h>
#include <boost/noncopyable.hpp>

#ifndef __H3_CRASH_REPOTER_H__
#define __H3_CRASH_REPOTER_H__

using namespace std;

    #pragma region CrashReporter
    class CrashReporter : boost::noncopyable
    {
    public:
        void EnableEmailDumpLogging ();
        void SetReportMessage       (char* inMessage);
        void UploadLogAndDump       ();

    private:
        bool    _crash_report = false;
		char* _message;
    };

	extern CrashReporter &GetCrashReporter();
    #pragma endregion

#endif //__H3_CRASH_REPOTER_H__