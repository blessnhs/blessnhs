
#include "stdafx.h"
#include "MSG_ACCEPT.h"

void MSG_ACCEPT::Execute(LPVOID Param)
{
	if(pSession != NULL)
	{
		pSession->SetConnected(TRUE);
	}
}