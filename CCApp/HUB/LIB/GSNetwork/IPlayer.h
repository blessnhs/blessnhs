#pragma once

#include "GSObject.h"

typedef GSNetwork::GSObject::GSObject GSObject;

class IPlayer : public GSObject
{
public:
	IPlayer(void);
	~IPlayer(void);

	class Account
	{
	public:
		Account() {}
		~Account() {}
	};
};

