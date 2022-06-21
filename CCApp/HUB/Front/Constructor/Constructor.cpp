#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSFront.h"
#include "CrashHandler/SFCrashHandler.h"
#include "../Timer/ContentsTimer.h"

FrontConstructor::FrontConstructor(void)
{
	//프로젝트 속성에 c/c++ => 코드 생성 항목에서 기본 런타임 검사를 기본값으로 설정해야함
	SFCrashHandler handler;
	handler.Install();
	handler.SetFHPtr(nullptr);

	SERVER.Initialize();
	PROXYHUB.Create(INI.HubSessionCount);

	GetContentsTimer().Start();
	
	printf("Server Start.................. \n");

	FrontCommand *_Command = new FrontCommand();
	
	while(TRUE)
	{
		CHAR command[256];
		memset(command, 0, sizeof(command));
		scanf_s("%s",command,(int)sizeof(command));

		_Command->Execute(command);
	}

	delete _Command;
}


FrontConstructor::~FrontConstructor(void)
{
}
