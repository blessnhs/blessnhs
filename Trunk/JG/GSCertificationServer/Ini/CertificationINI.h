#pragma once

#include "SingleTonHolder.h"

class CertificationINI : public Singleton<CertificationINI>
{
public:
	CertificationINI(void) {} 
	~CertificationINI(void) {}

	VOID Load();

	WCHAR Default_FrontIp[MAX_IP_ADDRESS];
	WORD  Default_FrontPort;

	DWORD Port ; 
	DWORD MaxUser ;

};

#define  INI Singleton<CertificationINI>::Instance()