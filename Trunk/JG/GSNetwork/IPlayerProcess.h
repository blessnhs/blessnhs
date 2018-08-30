#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "GSClient.h"

namespace GSNetwork {	namespace IPlayerProcess	{

class IPlayerProcess : public IProcess<GSCLIENT>
{
public:
	IPlayerProcess(void);
	~IPlayerProcess(void);
};

}	}

#define PLAYERPROCESS GSNetwork::IPlayerProcess::IPlayerProcess