#include "StdAfx.h"
#include "./IPlayerProcess.h"

#include "GSSerialize.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#define CHECK_ABUSE_TIME(NAME,PLAYER,TIME,DISCONNECT)	\
	if(pOwner->GetAbuseCheckTime(NAME,TIME) == true	)\
{\
	if(DISCONNECT)\
	pOwner->OnDisconnect();\
	return ;\
}\


namespace GSNetwork {	namespace IPlayerProcess	{

IPlayerProcess::IPlayerProcess(void)
{

}


IPlayerProcess::~IPlayerProcess(void)
{
}




}	}