#include "StdAfx.h"
#include "GSCommand.h"


GSCommand::GSCommand(void)
{
}


GSCommand::~GSCommand(void)
{
}


void GSCommand::AddCommand(std::wstring Name,Func _func)
{
	command[Name] = _func;
}

VOID GSCommand::Call(std::wstring Name,std::wstring arg)
{
	Func _func = command[Name];
	if(_func != NULL)
		_func(arg);
	else
		;
}