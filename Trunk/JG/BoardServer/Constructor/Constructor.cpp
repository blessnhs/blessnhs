#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSBoard.h"

BoardConstructor::BoardConstructor(void)
{
	SERVER.Initialize();
	
	wprintf(_T("Alloc Client Count %d\n"), SERVER.GetClientMgr().GetActiveSocketCount());

	BoardCommand _Command;
	
	while(TRUE)
	{
		WCHAR command[256];
		wscanf_s(_T("%s"),command,sizeof(command));

		printf("AliveCount %d\n", SERVER.GetClientMgr().GetActiveSocketCount());
	}


}


BoardConstructor::~BoardConstructor(void)
{
}
