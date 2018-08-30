#include "stdafx.h"
#include "MSG_PACKET_UDP.h"
#include "../GSPacket/Relay.h"
#include "../Process/Adapt.h"
#include "../process/IUdpProcess.h"

MSG_PACKET_UDP::MSG_PACKET_UDP()
{
	pSession = NULL;
}

void MSG_PACKET_UDP::Execute(LPVOID Param)
{
	if(pSession != NULL)
	{
		boost::shared_ptr<XDATA> pBuffer;

		while(pSession->m_PacketList.unsafe_size() != 0)
		{
			pSession->m_PacketList.try_pop(pBuffer);

			if(pSession->GetProcess() == NULL)
			{
				pSession->SetProcess( new IUdpProcess() );
			}

			pSession->GetProcess()->Process(pBuffer->m_Buffer.GetBuffer(),pBuffer->Length,pBuffer->MainId,pBuffer->SubId);
		}
	}
}
