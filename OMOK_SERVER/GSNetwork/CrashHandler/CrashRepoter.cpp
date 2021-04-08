
#include <string>
#include "CrashRepoter.h"

    #pragma region CrashReporter
 
    void CrashReporter::SetReportMessage(char* inMessage)
    {
        _message = inMessage;
        /*_message = "Message";*/
    }

    void CrashReporter::EnableEmailDumpLogging()
    {
        _crash_report = true;
    }

	CrashReporter &GetCrashReporter()
	{
		static CrashReporter _crashReporter;
		return _crashReporter;
	}

/*
    void UploadLogAndDump()
    {
        if (ServiceDirectors::GOptionConfigureCoordinator::Instance().IsLiveService())
        {
            UploadLiveLogAndDump();
        }
        else if (ServiceDirectors::GOptionConfigureCoordinator::Instance().IsQAService())
        {
            UploadQALogAndDump()
        }
    }

    void UploadLiveLogAndDump()
    {
        const string root = R"(c:\)";
        
        boost::filesystem::path cmd(R"(C:\Services\Startup\logbackup_live_game_schedule.cmd)");
        
        ::ShellExecute(NULL,
                       "open",
                       path.generic_string().c_str(),
                       NULL,
                       root.c_str(),
                       SW_NORMAL);
    }

    void UploadQALogAndDump()
    {
        const string root = R"(c:\)";
        const string cmd = ;


        boost::filesystem::path cmd(R"(C:\Services\Startup\logbackup_gq_game_schedule.cmd)");

        ::ShellExecute(NULL,
                       "open",
                       path.generic_string().c_str(),
                       NULL,
                       root.c_str(),
                       SW_NORMAL);
    }*/

    //CrashReporter::Instance().SendCrashReportMail(walker.GetFilePath(), dump.GetFilePath());
