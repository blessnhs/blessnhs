#pragma once

#include "IHandler.h"
#include <boost/shared_ptr.hpp>

namespace GSNetwork	{
		
class IMessage : public GSNetwork::GSObject::GSObject	
{
public:
	IMessage() {}
	~IMessage(){}

	IHandler<boost::shared_ptr<IMessage>>	*pHandler;
	LPVOID		Message;
	BYTE		Type;
	BYTE		SubType;

	virtual void Execute(LPVOID Param){};

};


}

#define IMESSAGE GSNetwork::IMessage

typedef boost::shared_ptr<GSNetwork::IMessage> IMessagePtr;
