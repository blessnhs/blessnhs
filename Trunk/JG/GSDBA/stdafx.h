// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

#include "Common.h"
#include "../GSPacket/Protocol.h"

#include "lock.h"
#include "GSGuard.h"
#include "GSIocp.h"

#include "GSSocket.h"
#include "GSMemPool.h"
#include "GSPacket.h"
#include "MiniDump.h"

#include "GSOdbc.h"
#include "DBProcess.h"

#include "GSBuffer.h"
#include "NetworkSession.h"
#include "NetWorkSessionMgr.h"

#define MAX_USER	100



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
