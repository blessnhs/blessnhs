#include "StdAfx.h"
#include "Constructor.h"

#include "../Command/Command.h"
#include "../Server/GSBoard.h"
#include "CrashHandler/SFCrashHandler.h"

BoardConstructor::BoardConstructor(void)
{
	//������Ʈ �Ӽ��� c/c++ => �ڵ� ���� �׸񿡼� �⺻ ��Ÿ�� �˻縦 �⺻������ �����ؾ���
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
