#include "CrashRepoter.h"
#include "Dump.h"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#pragma comment(lib, "Dbghelp.lib")

std::pair<std::string, std::string> CrashHandler::GenerateDumpFilePath()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
	std::string str(buffer);

    const string&& filename(buffer);

    boost::filesystem::path path = boost::filesystem::current_path();
    path.append("dumps");
    path.append(filename);

    path.replace_extension("dmp");
    const string&& dump = path.generic_string<std::string>();
    path.replace_extension("txt");
    const string&& cs = path.generic_string<std::string>();

    boost::filesystem::create_directory(path.branch_path().generic_string<std::string>());

    return std::make_pair(dump, cs);
}

    #pragma region OutdatedCrashHandler
    BOOL OutdatedCrashHandler::Install()
    {
        ::SetUnhandledExceptionFilter(OutdatedCrashHandler::Handle);

        return TRUE;
    }

    LONG OutdatedCrashHandler::Handle(LPEXCEPTION_POINTERS exptrs)
    {
        using namespace std;

/*
        if (::IsDebuggerPresent())
        {
            return ::UnhandledExceptionFilter(exptrs);
        }
*/

		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);

        const string&& filename(buffer);

        boost::filesystem::path path = boost::filesystem::current_path();
        path.append("dumps");
        path.append(filename);

        path.replace_extension("dmp");
        const string&& dump = path.generic_string<std::string>();
        path.replace_extension("txt");
        const string&& cs = path.generic_string<std::string>();

        boost::filesystem::create_directory(path.branch_path().generic_string<std::string>());

        const auto ph =  ::GetCurrentProcess();
        const auto pid = ::GetCurrentProcessId();

        #pragma region StackWalker
        OutdatedStackwalker walker(cs);
        walker.ShowCallstack(GetCurrentThread(), exptrs->ContextRecord);
        #pragma endregion

        #pragma region DUMP
        // Open a dump file
        HANDLE hFile = CreateFile(dump.c_str(),
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);

    //    SUPER_ASSERT(hFile != INVALID_HANDLE_VALUE);

        MINIDUMP_EXCEPTION_INFORMATION mei;
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = exptrs;
        mei.ClientPointers = true;

        // Dump exception information into the file
        MiniDumpWriteDump(
            ph,
            pid,
            hFile,
            MiniDumpNormal,
            &mei,
            NULL,
            NULL);

        CloseHandle(hFile);
        #pragma endregion

        return EXCEPTION_EXECUTE_HANDLER;
    }
    #pragma endregion

