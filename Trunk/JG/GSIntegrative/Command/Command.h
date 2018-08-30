#pragma once

#include "GSCommand.h"

class IntergretiveCommand : public GSCommand
{
public:
	IntergretiveCommand(void) { Initialize() ; }
	~IntergretiveCommand(void) {}

	void Initialize();
	void Execute(std::wstring _str);


	static VOID UserList(std::wstring);
};
