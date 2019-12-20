#pragma once
#include "SingleTonHolder.h"

class BoardINI : public Singleton<BoardINI>
{
public:
	BoardINI(void) {} 
	~BoardINI(void) {}

	VOID Load();

	WCHAR Default_FrontIp[MAX_IP_ADDRESS];
	WORD  Default_FrontPort;

	DWORD Port ; 
	DWORD MaxUser ;

};

#define  INI Singleton<BoardINI>::Instance()
