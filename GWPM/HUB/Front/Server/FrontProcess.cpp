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

VOID FrontProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, BOOL IsCompress,boost::shared_ptr<GSClient> Client)
{
	try
	{	//로그인 하지 않은 유저가 패킷을 요청 했을때
		// 버전이나 로그인 패킷이 아닌 경우 처리하지 않는다.
		if (MainProtocol != ID_PKT_VERSION_REQ && MainProtocol != ID_PKT_LOGIN_REQ)
		{
		}
#ifdef  _DEBUG
		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();
		std::string name = descriptor->FindValueByNumber(MainProtocol)->name();

		if (MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
			BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);
#endif //  _DEBUG
		
		//hub로 전송

		auto hub = PROXYHUB.GetHub(Client->GetId());
		if (hub != NULL)
		{
			if (hub->GetConnected() == FALSE)
				return;

			//압축해제를 하지않고 허브로 그냥 전달만 한다.
			hub->GetTCPSocket()->RelayPacket(MainProtocol, Client->GetId(), IsCompress,(BYTE*)Data, Length);
		}


	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

