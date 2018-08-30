#pragma once

#include "stdafx.h"

struct HEADER
{
	HEADER()
	{
		MainId = 0;
		SubId  = 0;
	}

	WORD MainId;
	WORD SubId;
};

