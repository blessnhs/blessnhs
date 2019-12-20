#pragma once
#include "GSObject.h"
#include "GSCommand.h"

class BoardCommand : public GSCommand
{
public:
	BoardCommand(void) { Initialize() ; }
	~BoardCommand(void) {}

	void Initialize();
	void Execute(std::wstring _str);


	static VOID UserList(std::wstring);


};
	