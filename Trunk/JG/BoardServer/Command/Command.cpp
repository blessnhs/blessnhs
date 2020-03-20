#include "Stdafx.h"
#include "command.h"

	void BoardCommand::Initialize()
	{
		command["l"] = &BoardCommand::UserList;
		command["r"] = &BoardCommand::RoomList;
	}

	void BoardCommand::Execute(std::string _str)
	{

		int Pos = _str.find(':');
		if(Pos == -1)
		{
			printf("Not Found Command\n");
			return ;
		}

		std::string Word = _str.substr(0,Pos);
		std::string Arg  = _str.substr(Pos,_str.size()-Pos);

		if(command.find(Word) != command.end())
		{
			Call(Word,_str);
		}
	}

	VOID BoardCommand::UserList(std::string)
	{
		printf("Current Client Count %d\n", SERVER.GetClientMgr().GetActiveSocketCount());
	}

	VOID BoardCommand::RoomList(std::string)
	{
		printf("Current Room Count %d\n", ROOMMGR.RoomCount());
	}

