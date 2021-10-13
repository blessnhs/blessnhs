#include "StdAfx.h"

#include "HubProcess.h"
#include "Base64.h"

using namespace google;

HubProcess::HubProcess(void)
{
}


HubProcess::~HubProcess(void)
{
}

VOID HubProcess::Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client)
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

	//	if(MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
	//		BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);

#endif

		GSCLIENT_PTR pPair = SERVER.GetClient(SubProtocol);
		if (pPair != NULL)
		{
			if (pPair->GetConnected() == FALSE)
				return;

			if (MainProtocol == ID_PKT_CLIENT_KICK)
			{
				CLIENT_KICK kick;
				SEND_PROTO_BUFFER(kick, pPair)
//				SERVER.Close(pPair->GetTCPSocket()->GetSocket());
			}
			else
				pPair->GetTCPSocket()->WritePacket(MainProtocol, 0, (BYTE*)Data, Length);
		}
	
	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

