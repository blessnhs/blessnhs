#include "StdAfx.h"

#include "FrontProcess.h"
#include "Base64.h"

using namespace google;

FrontProcess::FrontProcess(void)
{
}


FrontProcess::~FrontProcess(void)
{
}

VOID FrontProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client)
{
	try
	{	//로그인 하지 않은 유저가 패킷을 요청 했을때
		// 버전이나 로그인 패킷이 아닌 경우 처리하지 않는다.
		if (MainProtocol != ID_PKT_VERSION_REQ && MainProtocol != ID_PKT_LOGIN_REQ)
		{
		}

		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();
		std::string name = descriptor->FindValueByNumber(MainProtocol)->name();

		if(MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
			BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);

		
		//hub로 전송

		auto hub = PROXYHUB.GetHub();
		if (hub != NULL)
		{
			hub->GetTCPSocket()->WritePacket(MainProtocol, Client->GetId(), (BYTE*)Data, Length);
		}


	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

