#include "Stdafx.h"
#include "command.h"
#include <atomic>

extern atomic<int>		DebugCount;

	void FrontCommand::Initialize()
	{
		command["l"] = &FrontCommand::UserList;
		command["r"] = &FrontCommand::RoomList;
		command["c"] = &FrontCommand::CheckUserList;
		
	}

	void FrontCommand::Execute(std::string _str)
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

	VOID FrontCommand::UserList(std::string)
	{
		printf("Current Count %d Debug Count %d\n", SERVER.GetClientMgr().GetActiveSocketCount(), DebugCount.fetch_add(0));
	}

	VOID FrontCommand::RoomList(std::string)
	{
	
	}

	VOID FrontCommand::CheckUserList(std::string)
	{
		
	}

	
	VOID FrontCommand::ChannelList(std::string)
	{
	}
