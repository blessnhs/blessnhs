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

VOID HubProcess::Process(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client)
{
	try
	{	//로그인 하지 않은 유저가 패킷을 요청 했을때
		// 버전이나 로그인 패킷이 아닌 경우 처리하지 않는다.
		if (pBuffer->MainId != ID_PKT_VERSION_REQ && pBuffer->MainId != ID_PKT_LOGIN_REQ)
		{
		}
#ifdef  _DEBUG
		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();
		//std::string name = descriptor->FindValueByNumber(pBuffer->MainId)->name();

	//	if(MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
	//		BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);

#endif

		GSCLIENT_PTR pPair = SERVER.GetClient(pBuffer->Reserve2);
		if (pPair != NULL)
		{
			if (pPair->GetConnected() == FALSE)
				return;

			if (pBuffer->MainId == ID_PKT_CLIENT_KICK)
			{
			//	CLIENT_KICK kick;
			//	SEND_PROTO_BUFFER(kick, pPair,0)

				SERVER.Close(pPair->GetTCPSocket()->GetSocket());
			}
			else //그냥 전달만 한다.
				pPair->GetTCPSocket()->RelayPacket(pBuffer->MainId, pBuffer->SubId, pBuffer->IsCompress,(BYTE*)pBuffer->m_Buffer.GetBuffer(), pBuffer->m_Buffer.GetLength(), pBuffer->Reserve2);
		}
	
	}
	catch (int exception)
	{
		printf("handle exception %d\n", exception);
	}
}

