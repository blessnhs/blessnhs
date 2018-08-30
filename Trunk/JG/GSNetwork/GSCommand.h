#pragma once
 
#include <map>

class GSCommand
{
public:
	GSCommand(void);
	~GSCommand(void);

	typedef VOID (*Func)(std::wstring);

	void AddCommand(std::wstring Name,Func);
	VOID Call(std::wstring Name,std::wstring arg);

protected:

	std::map<std::wstring,Func> command;
};

