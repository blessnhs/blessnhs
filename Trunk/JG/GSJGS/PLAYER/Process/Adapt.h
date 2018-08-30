#pragma once

#include "IProcess.h"
#include "../NetworkSession.h"

namespace GSFrontServer {	namespace Adapt	{

class Adapt
{
public:
	Adapt(void);
	~Adapt(void);

	enum E_TYPE
	{
		PLAYER=0,
		FRONT_SERVER,
		FIELD_SERVER,
	};

	static IProcess<SESSION> *Create(E_TYPE nType);
};

}	}