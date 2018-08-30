#pragma once

#include "ICommand.h"

#include "MSG.h"

class MSG_TIME : public SvrMSG
{
public:
	MSG_TIME() {}
	~MSG_TIME(){}

	void Execute(LPVOID Param);
	
	void Undo(){}
};

typedef boost::shared_ptr<MSG_TIME> MSG_TIME_PTR;