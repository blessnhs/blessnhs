#include "StdAfx.h"
#include "command.h"
#include <string>
#include "../Player/PlayerManager.h"

void IntergretiveCommand::Initialize()
{
	command[_T("ll")] = &IntergretiveCommand::UserList;
}

void IntergretiveCommand::Execute(std::wstring _str)
{

	int Pos = _str.find(':');
	if(Pos == -1)
	{
		printf("Not Found Command\n");
		return ;
	}

	std::wstring Word = _str.substr(0,Pos);
	std::wstring Arg  = _str.substr(Pos,_str.size()-Pos);

	if(command.find(Word) != command.end())
	{
		Call(Word,_str);
	}
}

VOID IntergretiveCommand::UserList(std::wstring)
{
	PLAYERMGR.Print();
}
