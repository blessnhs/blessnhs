#pragma once
#include <stdio.h>
#include <windows.h>
#include <iostream>

#define MAX_LOG_LEN  2048

class GSLog
{
private:
	bool _display;

	CRITICAL_SECTION cs;
	HANDLE			handle_;
	std::string			_name;

public:

	GSLog(string name,bool display = false);


	void Open();

	void Close();

	~GSLog(void);


	bool			Write(const char* fmt, ...);
};

extern GSLog &SYSLOG();



