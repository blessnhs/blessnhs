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


};
	