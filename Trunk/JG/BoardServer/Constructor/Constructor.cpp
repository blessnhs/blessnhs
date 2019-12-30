#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSBoard.h"
#include "MiniDump.h"

BoardConstructor::BoardConstructor(void)
{
	GSBoard *_instance = new GSBoard;
	
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
