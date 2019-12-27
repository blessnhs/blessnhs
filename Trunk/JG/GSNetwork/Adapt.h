#pragma once

#include "IProcess.h"
#include "GSClient.h"

namespace GSNetwork {	namespace Adapt	{

class Adapt
{
public:
	Adapt(void);
	~Adapt(void);

	enum E_TYPE
	{
		PLAYER=0,
		_SERVER_1,
		_SERVER_2,
		_SERVER_3,
		_SERVER_4,
	};
};

}	}