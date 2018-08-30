#include "StdAfx.h"
#include "./IPlayerProcess.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/MainLogin.h"
#include "../../FieldServer/FiledServer.h"
#include "../Message/MSG_PLAYER_QUERY.h"
#include "../../Constructor/GSMainProcess.h"
#include "../NetworkSession.h"
#include "../Message/MSG_PLAYER_CLOSE.h"
#include "../../IOCP/ServerIOCP.h"
#include "../../DBProxy/DBProcess.h"
#include "../../Allocator/GSAllocator.h"
#include "../GSPacket/Protocol.h"
#include "../Sender/Sender.h"
#include "GSSerialize.h"
#include "../../LOG/FrontLog.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"


#define CHECK_ABUSE_TIME(NAME,PLAYER,TIME,DISCONNECT)	\
	if(pOwner->GetAbuseCheckTime(NAME,TIME) == true	)\
{\
	if(DISCONNECT)\
	pOwner->OnDisconnect();\
	return ;\
}\


namespace GSFrontServer {	namespace IPlayerProcess	{

IPlayerProcess::IPlayerProcess(void)
{
}


IPlayerProcess::~IPlayerProcess(void)
{
}

VOID IPlayerProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}



}	}