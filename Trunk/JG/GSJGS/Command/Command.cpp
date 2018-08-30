#include "StdAfx.h"
#include "command.h"
#include "../PLAYER/Container/PlayerContainer.h"

namespace GSFrontServer	{	


	void FrontCommand::Initialize()
	{
		command[_T("ll")] = &FrontCommand::UserList;
		command[_T("lc")] = &FrontCommand::CheckUserList;
	}

	void FrontCommand::Execute(std::wstring _str)
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

	VOID FrontCommand::UserList(std::wstring)
	{
		PLAYERMGR.Print();
	}

	VOID FrontCommand::CheckUserList(std::wstring)
	{
		PLAYERMGR.CheckUserList();
	}

}	