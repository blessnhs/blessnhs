#include "StdAfx.h"
#include "CertificationConstructor.h"

#include "../DB/DBProxy/DBProcessContainer.h"
#include "../Command/Command.h"

#include "../server/GSCertification.h"

CertificationConstructor::CertificationConstructor(void)
{
	GSCertification *_instance = new GSCertification;

	DBPROCESSCONTAINER_CER.Initialize();
	
	CertificationCommand _Command;

	while(TRUE)
	{
		WCHAR command[256];
		wscanf(_T("%s"),command);

		printf("AliveCount %d\n",_instance->GetClientMgr().GetActiveSocketCount());
	}


}


CertificationConstructor::~CertificationConstructor(void)
{
}
