// BoardServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"

#include "./Command/Command.h"
#include "Constructor/Constructor.h"
#include "MinidumpHelp.h"

int main()
{
	MinidumpHelp dump;

	dump.install_self_mini_dump();

	BoardCommand _Command;
	BoardConstructor			_Constructor;
	
	return 0;

}
