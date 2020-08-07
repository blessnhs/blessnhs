#include "CrashRepoter.h"
#include "Dump.h"
#include <iostream>
#include <time.h>
#pragma comment(lib, "Dbghelp.lib")

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y_%m_%d_%X", &tstruct);

	return buf;
}

std::string replace_all(
	__in const std::string &message,
	__in const std::string &pattern,
	__in const std::string &replace
) {
	std::string result = message;
	std::string::size_type pos = 0;
	std::string::size_type offset = 0;
	while ((pos = result.find(pattern, offset)) != std::string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}
	return result;
}


std::pair<std::string, std::string> CrashHandler::GenerateDumpFilePath()
{
	char path_c[1024];
	GetCurrentDirectory(512, path_c);

	string filename = currentDateTime();

	filename = replace_all(filename, ":", "_");

	string path = path_c;

	string file1 = path + "\\" + filename + ".dmp";
	string file2 = path + "\\" + filename + ".txt";

    return std::make_pair(file1, file2);
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
		char path_c[1024];
		GetCurrentDirectory(512, path_c);

		string filename = currentDateTime();

		filename = replace_all(filename, ":", "_");

		string path = path_c;

		string dump = path + "\\" + filename + ".dmp";
		string cs = path + "\\" + filename + ".text";


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

