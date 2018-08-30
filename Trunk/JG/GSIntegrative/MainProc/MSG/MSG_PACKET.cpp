#include "stdafx.h"
#include "MSG_PACKET.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/relay.h"
#include "../Process/Adapt.h"

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

				switch(pBuffer->MainId)
				{
				case ID_RG_PKT_REGIST_REQ:
					pSession->SetProcess(Adapt::Create(Adapt::RELAY_SERVER));
					pSession->SetType(Adapt::RELAY_SERVER);
					break;
				case ID_FG_PKT_REGIST_RES:
					pSession->SetProcess(Adapt::Create(Adapt::FRONT_SERVER));
					pSession->SetType(Adapt::FIELD_SERVER);
					break;
				default:
					{
						return ;
					}
					break;
				}
			}
			pSession->GetProcess()->Process(pBuffer->m_Buffer.GetBuffer(),pBuffer->Length,pBuffer->MainId,pBuffer->SubId);
		}

	}
}
