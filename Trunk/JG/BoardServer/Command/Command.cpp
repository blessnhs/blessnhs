#include "Stdafx.h"
#include "command.h"

	void BoardCommand::Initialize()
	{
		command[_T("ll")] = &BoardCommand::UserList;
	}

	void BoardCommand::Execute(std::wstring _str)
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

	VOID BoardCommand::UserList(std::wstring)
	{

	}

