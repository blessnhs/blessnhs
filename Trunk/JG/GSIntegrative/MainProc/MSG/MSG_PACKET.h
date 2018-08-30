#pragma once
#include "../NetworkSession/NetworkSession.h"
#include "ICommand.h"
#include "../Process/Adapt.h"
#include "MSG.h"

class MSG_PACKET : public SvrMSG
{
public:
	MSG_PACKET() {}
	~MSG_PACKET(){}

	SESSION		*pSession;

	void Execute(LPVOID Param);
	void Undo(){}
};

typedef boost::shared_ptr<MSG_PACKET> MSG_PACKET_PTR;