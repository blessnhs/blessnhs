#include "StdAfx.h"
#include "GSClientModule.h"

namespace GSNetwork { namespace GSEventSelect {	namespace GSClientModule	{

GSClientModule::GSClientModule(VOID)
{
	IsUdp = FALSE;
}

GSClientModule::~GSClientModule(VOID)
{
}

BOOL			GSClientModule::BeginUdp(USHORT remotePort)
{
#if defined(_EXTERNAL_RELEASE_)
	if (remotePort <= 0)
		return FALSE;
#endif

	if (!mUDPSession.Initialize())
	{
		End();

		return FALSE;
	}

	if (!mUDPSession.CreateUdpSocket(remotePort))
	{
		End();

		return FALSE;
	}

	if (!GSEventSelect::Initialize(mUDPSession.GetSocket()))
	{
		End();

		return FALSE;
	}

	IsUdp = TRUE;

	return TRUE;
}


BOOL GSClientModule::BeginTcp(LPSTR remoteAddress, USHORT remotePort)
{
	if (!remoteAddress || remotePort <= 0)
		return FALSE;

	if (!mTCPSession.Initialize())
	{
		End();

		return FALSE;
	}

	if (!mTCPSession.CreateTcpSocket())
	{
		End();

		return FALSE;
	}

	if (!GSEventSelect::Initialize(mTCPSession.GetSocket()))
	{
		End();

		return FALSE;
	}

	if (!mTCPSession.Connect(remoteAddress, remotePort))
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL GSClientModule::End(VOID)
{
	if(IsUdp == TRUE)
		mUDPSession.Termination();
	else 
		mTCPSession.Termination();

	GSEventSelect::Termination();

	return TRUE;
}

BOOL GSClientModule::GetLocalIP(string pIP)
{
	int port = 0;
	if(IsUdp == TRUE)
		return mUDPSession.GetLocalIP(pIP, port);
	else
	    return mTCPSession.GetLocalIP(pIP, port);
}

USHORT GSClientModule::GetLocalPort(VOID)
{
	if(IsUdp == TRUE)
		return mUDPSession.GetLocalPort();
	else
	    return mTCPSession.GetLocalPort();
}

BOOL GSClientModule::ReadPacket(WORD &protocol, BYTE *packet, WORD &packetLength)
{
	if(m_ReadPacketQueue.size() == 0) return FALSE;

	std::shared_ptr<READ_PACKET_INFO> pData = m_ReadPacketQueue.front();

	CopyMemory(packet,pData->Data,sizeof(pData->Data));
	protocol = (WORD)pData->Protocol;
	packetLength = (WORD)pData->DataLength;
	
	if(pData != NULL)
	{
		m_ReadPacketQueue.pop();

		return TRUE;
	}

	return FALSE;
}


BOOL GSClientModule::WritePacket(WORD protocol, WORD Subprotocol,const BYTE *packet, WORD packetLength)
{
	if (!mTCPSession.WritePacket(protocol,Subprotocol, packet, packetLength,0))
		return FALSE;

	if (!mTCPSession.WriteComplete())
		return FALSE;

	return TRUE;
}

VOID GSClientModule::OnIoRead(VOID)
{
	BYTE	Packet[MAX_BUFFER_LENGTH]	= {0,};
	DWORD	PacketLength				= 0;

	WORD	MainProtocol				= 0;
	WORD	SubProtocol					= 0;

	if (IsUdp)
	{
		CHAR	RemoteAddress[32]	= {0,};
		USHORT	RemotePort			= 0;

		if (mUDPSession.ReadFromPacketForEventSelect(RemoteAddress, RemotePort))
		{
			while (mUDPSession.GetPacket(RemoteAddress, RemotePort, MainProtocol,SubProtocol, Packet, PacketLength))
			{
				std::shared_ptr<READ_PACKET_INFO> pReadData(new READ_PACKET_INFO());

				CopyMemory(pReadData->Data,Packet,sizeof(pReadData->Data));
				pReadData->DataLength = PacketLength;
				pReadData->Object = this;
				
				m_ReadPacketQueue.push(pReadData); 
			}

		}
	}
	else
	{
		if (mTCPSession.ReadPacketForEventSelect())
		{
			while (mTCPSession.GetPacket())
			{
				std::shared_ptr<READ_PACKET_INFO> pReadData(new READ_PACKET_INFO());

				CopyMemory(pReadData->Data,Packet,sizeof(pReadData->Data));
				pReadData->DataLength = PacketLength;
				pReadData->Object = this;
				
				m_ReadPacketQueue.push(pReadData); 
			}
		}
	}
}

}	}	}