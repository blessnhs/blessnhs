#include "StdAfx.h"
#include "Constructor.h"

#include "../DB/DBProxy/DBProcessContainer.h"
#include "../Command/Command.h"

#include "../server/GSBoard.h"

BoardConstructor::BoardConstructor(void)
{
	GSBoard *_instance = new GSBoard;

	DBPROCESSCONTAINER_CER.Initialize();
	
	BoardCommand _Command;

	while(TRUE)
	{
		WCHAR command[256];
		wscanf_s(_T("%s"),command,sizeof(command));

		printf("AliveCount %d\n",_instance->GetClientMgr().GetActiveSocketCount());
	}


}


BoardConstructor::~BoardConstructor(void)
{
}
