#include "Stdafx.h"
#include "command.h"
#include <atomic>

extern atomic<int>		DebugCount;

	void BoardCommand::Initialize()
	{
		command["l"] = &BoardCommand::UserList;
		command["r"] = &BoardCommand::RoomList;
		command["c"] = &BoardCommand::CheckUserList;
		
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
		printf("Current Count %d user count %d Debug Count %d\n", SERVER.GetClientMgr().GetActiveSocketCount(), PLAYERMGR.Count(), DebugCount.fetch_add(0));
	}

	VOID BoardCommand::RoomList(std::string)
	{
	
	}

	VOID BoardCommand::CheckUserList(std::string)
	{
		PLAYERMGR.CheckUserList();
	}

	
	VOID BoardCommand::ChannelList(std::string)
	{
		CHANNELMGR.Report();
	}
