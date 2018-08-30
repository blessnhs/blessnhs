// GSIntegrative.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "./Command/Command.h"
#include "./Constructor/GSConstructor.h"

int _tmain(int argc, _TCHAR* argv[])
{
	IntergretiveCommand				_Command;
	GSConstructor					_Constructor;

	while(TRUE)
	{
		WCHAR command[256];
		wscanf(_T("%s"),command);

		_Command.Execute(command);
	}

	return 0;
}

