// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

#include "GSObject.h"
#include "Common.h"

#include "lock.h"
#include "GSGuard.h"

#include "GSSocket.h"
#include "GSMemPool.h"
#include "GSPacket.h"
#include "MiniDump.h"

#include "GSClient.h"
#include "SERVER/GSJGServer.h"
#include "constructor/GSIniContext.h"
#include "GSMainProcess.h"

using namespace std;


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
