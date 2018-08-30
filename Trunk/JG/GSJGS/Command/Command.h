#pragma once

#include "GSObject.h"
#include "GSCommand.h"

namespace GSFrontServer	{	

class FrontCommand : public GSCommand
{
public:
	FrontCommand(void) { Initialize() ; }
	~FrontCommand(void) {}

	void Initialize();
	void Execute(std::wstring _str);


	static VOID UserList(std::wstring);
	static VOID CheckUserList(std::wstring);


};

}	