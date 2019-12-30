#pragma once
#include "IMessage.h"

class SvrMSG : public IMESSAGE
{
public:
	SvrMSG() {}
	~SvrMSG(){}

	int SubType;

	virtual void Execute(LPVOID Param){};
};

typedef boost::shared_ptr<SvrMSG> SvrMSGPtr;