#pragma once

#include "GSObject.h"


class ICommand
{
public:
	ICommand() {}
	~ICommand(){}

	virtual void Execute(LPVOID Param) = 0;
	virtual void Undo() = 0;
};