#pragma once

#include "./IMessage.h"
#include "MSG_PLAYER.h"

class MSG_PLAYER : public IMessage 
{
public:
	MSG_PLAYER() {}
	~MSG_PLAYER(){}

	BYTE SubType;
};
