#pragma once
 
#include <map>

class GSCommand
{
public:
	GSCommand(void);
	~GSCommand(void);

	typedef VOID (*Func)(std::string);

	void AddCommand(std::string Name,Func);
	VOID Call(std::string Name,std::string arg);

protected:

	std::map<std::string,Func> command;
};

