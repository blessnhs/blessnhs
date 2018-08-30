#include "stdafx.h"
#include "MSG_PACKET.h"
#include "../GSPacket/Relay.h"
#include "../Process/Adapt.h"

MSG_PACKET::MSG_PACKET()
{
	pSession = NULL;
	pSession_UDP = NULL;

}

void MSG_PACKET::Execute(LPVOID Param)
{
	if(pSession != NULL)
	{
		boost::shared_ptr<XDATA> pBuffer;

		while(pSession->m_PacketList.unsafe_size() != 0)
		{
			pSession->m_PacketList.try_pop(pBuffer);

			if(pSession->GetProcess() == NULL)
			{
				PKT_REGIST_REQ *req = (PKT_REGIST_REQ * )pBuffer->m_Buffer.GetBuffer();

				switch(MainProtocol)
				{
				case ID_GR_PKT_REGIST_RES:
					pSession->SetProcess(Adapt::Create(Adapt::RELAY_SERVER));
					pSession->SetType(Adapt::RELAY_SERVER);
					break;
			
				default:
					{
						pSession->SetProcess(Adapt::Create(Adapt::TCP_CLIENT));
						pSession->SetType(Adapt::TCP_CLIENT);
					}
					break;
				}
			}
			pSession->GetProcess()->Process(pBuffer->m_Buffer.GetBuffer(),pBuffer->Length,pBuffer->MainId,pBuffer->SubId);
		}
	}
}
