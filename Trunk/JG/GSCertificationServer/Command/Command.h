#pragma once

#include "GSObject.h"
#include "GSCommand.h"

class CertificationCommand : public GSCommand
{
public:
	CertificationCommand(void) { Initialize() ; }
	~CertificationCommand(void) {}

	void Initialize();
	void Execute(std::wstring _str);


	static VOID UserList(std::wstring);


};
	