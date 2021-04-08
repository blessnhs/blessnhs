// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define USE_MSSQL
#define _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING

#include <stdio.h>
#include <tchar.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <queue>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <rpc.h>
#include <windows.h>
#include <iostream>

#include <time.h>
#include <boost/make_shared.hpp>


#include "CLI.GS.pb.h"
//위 순서를 바꾸면 대란이 일어남
//property = > c++ = > language = > conformance->no
//singleton 에러 해결 방법

#include "MongoDB.h"

//
#include "GSObject.h"
#include "Common.h"
//
#include "lock.h"
#include "GSGuard.h"

#include "GSSocket.h"
#include "GSMemPool.h"
#include "GSPacket.h"
#include "MiniDump.h"

#include "GSOdbc.h"
#include "Log/Log.h"
#include "SingleTonHolder.h"
#include "GSAllocator.h"
#include "GSMainProcess.h"

#include "DB/DBProxy/DBProcess.h"

#include "./DB/DBProxy/DBProcessContainer.h"
#include "./DB/DBJob/DBContext.h"

#include "./PLAYER/PlayerContainer.h"
#include "./Room/RoomContainer.h"

#include "MessageMapDef.h"

#include "./Server/GSHub.h"

#include "./MainProcess/MSG/MSG_PLAYER_QUERY.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <boost/make_shared.hpp>

#include "CLI.GS.pb.h"
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
