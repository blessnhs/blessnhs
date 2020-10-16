#pragma once
#include "GSCommand.h"

class BoardCommand : public GSCommand
{
public:
	BoardCommand(void) { Initialize() ; }
	~BoardCommand(void) {}

	void Initialize();
	void Execute(std::string _str);


	static VOID UserList(std::string);
	static VOID CheckUserList(std::string);
	static VOID RoomList(std::string);
	static VOID ChannelList(std::string);

};
	