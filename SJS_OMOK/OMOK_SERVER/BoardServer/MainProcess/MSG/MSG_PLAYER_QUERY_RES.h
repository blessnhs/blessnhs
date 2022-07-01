#pragma once

#include "ICommand.h"
#include "IMessage.h"

namespace Board	{

template<class T>
class MSG_PLAYER_QUERY_RES : public IMessage
{
public:
	MSG_PLAYER_QUERY_RES()  {}
	~MSG_PLAYER_QUERY_RES() {}

	T			*pResponse;
	WORD		Result;

	void Execute(LPVOID Param)
	{
	}

	void Undo(){}
};

}