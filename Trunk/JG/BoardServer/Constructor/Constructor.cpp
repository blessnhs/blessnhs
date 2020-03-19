#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSBoard.h"

BoardConstructor::BoardConstructor(void)
{
	SERVER.Initialize();
	
	printf(("Alloc Client Count %d\n"), SERVER.GetClientMgr().GetActiveSocketCount());

	BoardCommand _Command;
	
	while(TRUE)
	{
		CHAR command[256];
		scanf_s("%s",command,sizeof(command));

		printf("AliveCount %d\n", SERVER.GetClientMgr().GetActiveSocketCount());
	}


}


BoardConstructor::~BoardConstructor(void)
{
}
