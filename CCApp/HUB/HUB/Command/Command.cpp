#include "Stdafx.h"
#include "command.h"
#include <atomic>

extern atomic<int>		DebugCount;

	void HubCommand::Initialize()
	{
		command["l"] = &HubCommand::UserList;
		command["r"] = &HubCommand::RoomList;
		command["c"] = &HubCommand::CheckUserList;
		
	}

	void HubCommand::Execute(std::string _str)
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

	VOID HubCommand::UserList(std::string)
	{
		printf("Current Count %d user count %d Debug Count %d\n", SERVER.GetClientMgr().GetActiveSocketCount(), PLAYERMGR.Count(), DebugCount.fetch_add(0));
	}

	VOID HubCommand::RoomList(std::string)
	{
		ROOMMGR.Print();
	}

	VOID HubCommand::CheckUserList(std::string)
	{
		PLAYERMGR.CheckUserList();
	}

	
	VOID HubCommand::ChannelList(std::string)
	{
	}
