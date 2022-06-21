#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSHub.h"
#include "CrashHandler/SFCrashHandler.h"
#include "../Timer/ContentsTimer.h"

HubConstructor::HubConstructor(void)
{
	//프로젝트 속성에 c/c++ => 코드 생성 항목에서 기본 런타임 검사를 기본값으로 설정해야함
	SFCrashHandler handler;
	handler.Install();
	handler.SetFHPtr(nullptr);

	SERVER.Initialize();

	GetContentsTimer().Start();
	
	printf("Server Start.................. \n");

	HubCommand *_Command = new HubCommand();
	
	while(TRUE)
	{
		CHAR command[256];
		memset(command, 0, sizeof(command));
		scanf_s("%s",command,(int)sizeof(command));

		_Command->Execute(command);
	}

	delete _Command;
}


HubConstructor::~HubConstructor(void)
{
}
