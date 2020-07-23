#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSBoard.h"
#include "CrashHandler/SFCrashHandler.h"

BoardConstructor::BoardConstructor(void)
{
	//프로젝트 속성에 c/c++ => 코드 생성 항목에서 기본 런타임 검사를 기본값으로 설정해야함
	SFCrashHandler handler;
	handler.Install();
	handler.SetFHPtr(nullptr);

	SERVER.Initialize();
	
	printf(("Alloc Client Count %d\n"), SERVER.GetClientMgr().ConnectableSocketCount());
	printf("Server Start.................. \n");

	BoardCommand _Command;
	
	while(TRUE)
	{
		CHAR command[256];
		scanf_s("%s",command,sizeof(command));

		_Command.Execute(command);
	}


}


BoardConstructor::~BoardConstructor(void)
{
}
