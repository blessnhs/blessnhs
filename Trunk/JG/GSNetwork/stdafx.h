// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <iostream>
#include <tchar.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <algorithm> 
#include <process.h>


#include "PacketBuffer.h"
#include "core.h"
#include "GSGuard.h"
#include "GSMemPool.h"
#include "GSObject.h"

using namespace std;

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//#define REMOVE_BOOST


#pragma warning(disable : 4101)

// TODO: reference additional headers your program requires here
