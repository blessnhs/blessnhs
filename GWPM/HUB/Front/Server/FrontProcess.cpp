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
	{	//�α��� ���� ���� ������ ��Ŷ�� ��û ������
		// �����̳� �α��� ��Ŷ�� �ƴ� ��� ó������ �ʴ´�.
		if (MainProtocol != ID_PKT_VERSION_REQ && MainProtocol != ID_PKT_LOGIN_REQ)
		{
		}

		const google::protobuf::EnumDescriptor* descriptor = PROTOCOL_descriptor();
		std::string name = descriptor->FindValueByNumber(MainProtocol)->name();

		if(MainProtocol != ID_PKT_ROOM_LIST_REQ && MainProtocol != ID_PKT_PRAY_MESSAGE_REQ)
			BLOG("%s MainProtocol %s Length %d\n", __FUNCTION__, name.c_str(), Length);

		
		//hub�� ����

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

