#include "StdAfx.h"
#include "GSLog.h"

GSLog::GSLog(string name,bool display)
{
	_display = display;
	_name = name;
	InitializeCriticalSection(&cs);
}


void GSLog::Open()
{

	SYSTEMTIME		sysTime;
	::GetLocalTime(&sysTime);

	CHAR			time_log[256];

	sprintf_s(time_log, 256, "%d_%d_%d_%s.txt",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, _name.c_str());

	handle_ = CreateFile(time_log, FILE_APPEND_DATA, 0, 0,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);


	if (handle_ == INVALID_HANDLE_VALUE) { return; }

	SetFilePointer(handle_, 0, NULL, FILE_END);
}

void GSLog::Close()
{
	if (handle_ != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(handle_);
		handle_ = INVALID_HANDLE_VALUE;
	}
}

GSLog::~GSLog(void)
{
	DeleteCriticalSection(&cs);
}

HANDLE			handle_;

bool			GSLog::Write(const char* fmt, ...)
{
	EnterCriticalSection(&cs);

	Open();

	if (handle_ == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&cs);
		return FALSE;
	}

	CHAR			arg_buffer[MAX_LOG_LEN];

	va_list ap;

	va_start(ap, fmt);
	vsprintf_s(arg_buffer, MAX_LOG_LEN, fmt, ap);
	va_end(ap);

	SYSTEMTIME		sysTime;
	::GetLocalTime(&sysTime);

	CHAR			time_log[MAX_LOG_LEN + MAX_LOG_LEN];

	sprintf_s(time_log, MAX_LOG_LEN + MAX_LOG_LEN, "[ %04d-%02d-%02d %02d:%02d:%02d:%03d ] : %s",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, arg_buffer);

	DWORD	write_len = 0;
	::WriteFile(handle_, time_log, (DWORD)(strlen(time_log) * sizeof(CHAR)), &write_len, NULL);

	if (_display == true)
		printf(("%s\n"), time_log);

	Close();

	LeaveCriticalSection(&cs);

	return TRUE;
}


extern GSLog &SYSLOG()
{
	static GSLog g_Log("system",false);
	return g_Log;
}
