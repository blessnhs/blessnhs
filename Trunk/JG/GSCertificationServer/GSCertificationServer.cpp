// GSCertificationServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "server/GSCertification.h"
#include "./Command/Command.h"
#include "Constructor/CertificationConstructor.h"

int _tmain(int argc, _TCHAR* argv[])
{

	CertificationCommand _Command;
	CertificationConstructor			_Constructor;

	while(TRUE)
	{
		WCHAR command[256];
		wscanf(_T("%s"),command);

		_Command.Execute(command);
	}

	return 0;
}

