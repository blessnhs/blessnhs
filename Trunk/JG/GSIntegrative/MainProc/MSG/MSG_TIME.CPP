
#include "stdafx.h"
#include "MSG_TIME.h"

#include "../PLAYER/PlayerManager.h"

void MSG_TIME::Execute(LPVOID Param)
{
	PLAYERMGR.CheckCloseJob();
}
