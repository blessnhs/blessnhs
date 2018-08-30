#pragma once

struct MultiThread
{
	MultiThread()
	{
		InitializeCriticalSection(&cs);
	}
	~MultiThread()
	{
		DeleteCriticalSection(&cs);
	}

	VOID Lock()
	{
		EnterCriticalSection(&cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&cs);
	}

	CRITICAL_SECTION cs;

};

struct SingleThread
{
	SingleThread()
	{
	}
	~SingleThread()
	{
	}

	VOID Lock()
	{
	}

	VOID UnLock()
	{

	}
};

#define MAX_LOG_LEN	256


template<class CreationPolicy>
class GSLog : public CreationPolicy
{
public:
	GSLog(LPCTSTR file_name)
	{

		SYSTEMTIME		sysTime;
		::GetLocalTime( &sysTime );

		TCHAR			time_log[ MAX_LOG_LEN+MAX_LOG_LEN ];

		_stprintf_s( time_log, MAX_LOG_LEN, _T( "[ %04d%02d%02d%02d%02d%02d%03d ] %s "), 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,file_name );


		handle_=CreateFile(time_log, GENERIC_READ | GENERIC_WRITE, 0, 0,       
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 


		if( handle_ == INVALID_HANDLE_VALUE )		{	return ;		}

		SetFilePointer( handle_, 0, NULL, 0 );
	}

	~GSLog(void)
	{
		if( handle_ != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( handle_ );
			handle_ = INVALID_HANDLE_VALUE;
		}
	}

	TCHAR			msg[ MAX_LOG_LEN ];
	HANDLE			handle_;			

	bool			Write( LPCTSTR pStrFormat, ... )
	{
		if( handle_ == INVALID_HANDLE_VALUE )	{	return INVALID_FILE_SIZE;		}

		Lock();

		//	로그를 조합합니다. -----------------------------------------------------
		va_list args;
		va_start( args, pStrFormat );
		_vstprintf_s( msg, pStrFormat, args );
		va_end( args );
		//--------------------------------------------------------------------------

		// 로그 기혹 위치 추가 ---------------------------------------------------
		int	iMsgLen	= _tcslen( msg );

		SYSTEMTIME		sysTime;
		::GetLocalTime( &sysTime );

		TCHAR			time_log[ MAX_LOG_LEN ];

		_stprintf_s( time_log, MAX_LOG_LEN, _T( "[ %04d-%02d-%02d %02d:%02d:%02d:%03d ] " ), 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds );

		DWORD	write_len	= 0;
		::WriteFile( handle_, time_log, ( DWORD )::_tcslen( time_log ) * sizeof(TCHAR),&write_len, NULL );
		::WriteFile( handle_, msg, ( DWORD )::_tcslen( msg ) * sizeof(TCHAR),&write_len, NULL );

		printf(("%S%S\n"),time_log,msg);

		UnLock();

		return TRUE;
	}


};
 
