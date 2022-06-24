#include "stdafx.h"
#include "MSG_PACKET_UDP.h"
#include "IUdpProcess.h"

MSG_PACKET_UDP::MSG_PACKET_UDP()
{
	
}

void MSG_PACKET_UDP::Execute(LPVOID Param)
{
	if(pClient != NULL)
	{
		boost::shared_ptr<XDATA> pBuffer;

		while(pClient->GetUDPSocket()->m_PacketList.unsafe_size() != 0)
		{
			pClient->GetUDPSocket()->m_PacketList.try_pop(pBuffer);

			if(pClient->GetProcess() == NULL)
			{
			//	pClient->SetProcess( new IUdpProcess() );
			}

			pClient->GetProcess()->Process(pBuffer,pClient);
		}
	}
}
