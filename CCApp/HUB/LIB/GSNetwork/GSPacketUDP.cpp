#include "StdAfx.h"
#include "GSPacketUDP.h"
#include "GSCrypt.h"
#include "GSPacket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketUDP	{

	typedef GSNetwork::GSCrypt::GSCrypt	GSCrypt;

GSPacketUDP::GSPacketUDP(void)
{
	SetSocketType(SOCKET_UDP);
}

GSPacketUDP::~GSPacketUDP(void)
{
}

BOOL	GSPacketUDP::Initialize(VOID)
{
	CThreadSync Sync;

	GSPacket::Initialize();

	return GSSocketUDP::Initialize();
}

BOOL	GSPacketUDP::Termination(VOID)
{
	CThreadSync Sync;

	GSPacket::Termination();

#ifndef CLIENT_MODULE
	m_PacketList.clear();
#endif
	return GSSocketUDP::Termination();
}


BOOL	GSPacketUDP::ReadFromPacketForIocp(LPSTR& remoteAddress, USHORT &remotePort, DWORD readLength)
{
	CThreadSync Sync;

	if (!GSSocketUDP::ReadFromForIocp(remoteAddress, remotePort, m_PacketBuffer + m_RemainLength, readLength))
		return FALSE;

	m_RemainLength	+= readLength;

	//return getPacket(remoteAddress, remotePort, protocol, packet, packetLength);
	return TRUE;

}

BOOL	GSPacketUDP::ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT &remotePort)
{
	CThreadSync Sync;

	DWORD ReadLength = 0;

	if (!GSSocketUDP::ReadFromForEventSelect(remoteAddress, remotePort, m_PacketBuffer + m_RemainLength, ReadLength))
		return FALSE;

	m_RemainLength	+= ReadLength;

	//return getPacket(remoteAddress, remotePort, protocol, packet, packetLength);
	return TRUE;

}

VOID	GSPacketUDP::MakePacket(DWORD dwDataLength,WORD &Mainprotocol, WORD &Subprotocol,DWORD &dataLength)
{
	WORD  MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;
	BYTE Packet[MAX_BUFFER_LENGTH] = {0,};

	LPSTR	RemoteAddress="";
	USHORT	RemotePort = 0;
	
	if (ReadFromPacketForIocp(RemoteAddress,RemotePort,dwDataLength))
	{
		while (GetPacket(RemoteAddress,RemotePort,MainProtocol,SubProtocol, Packet, dwDataLength))
		{
			boost::shared_ptr<XDATA> pBuffer = GetGSBufferPool().alloc();
			pBuffer->m_Buffer.SetBuffer(Packet,dwDataLength);
			pBuffer->MainId = MainProtocol;
			pBuffer->Length = dwDataLength;
			pBuffer->LemoteAddress = RemoteAddress;
			pBuffer->RemotePort = RemotePort;
			m_PacketList.push(pBuffer);

			MakeMsg(MainProtocol,SubProtocol,dwPacketLength);
		}
	}
}


BOOL	GSPacketUDP::WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, WORD MainProtocol, WORD SubProtocol,const BYTE *packet, WORD packetLength)
{
	CThreadSync Sync;

	if (!packet)
		return FALSE;

	WORD PacketLength = sizeof(WORD)  + 
		sizeof(WORD)                  + 
		sizeof(WORD)                  + 
		sizeof(DWORD)                 + 
		packetLength;

	if (PacketLength >= MAX_BUFFER_LENGTH)
	{
		return FALSE;
	}

	m_CurrentPacketNumber++;

	BYTE TempBuffer[MAX_BUFFER_LENGTH] = {0,};
	
	memcpy(TempBuffer, &PacketLength, sizeof(WORD));
	
	memcpy(TempBuffer + 
		sizeof(WORD), 
		&MainProtocol, sizeof(WORD));
	
	memcpy(TempBuffer + 
		sizeof(WORD)  + 
		sizeof(WORD), 
		&SubProtocol, sizeof(WORD));

	memcpy(TempBuffer + 
		sizeof(WORD)  + 
		sizeof(WORD)  + 
		sizeof(WORD), 
		&m_CurrentPacketNumber, sizeof(DWORD));

	memcpy(TempBuffer + 
		sizeof(WORD)  + 
		sizeof(WORD)  +
		sizeof(WORD)  + 
		sizeof(DWORD),
		packet, packetLength);

//	GSCrypt::Encrypt(TempBuffer + sizeof(DWORD), TempBuffer + sizeof(DWORD), PacketLength - sizeof(DWORD));

	// WriteQueue를 이용해서 패킷이 전송 완료가 되었을까지 메모리를 살려둔다.

#ifndef CLIENT_MODULE
	//굳이 넣을 필요 없음

	/*boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(m_WritePool.alloc());

	CopyMemory(pWriteData->Data,TempBuffer,sizeof(pWriteData->Data));
	pWriteData->DataLength = PacketLength;
	pWriteData->Object = this;

	m_WrietQueue.push(pWriteData); */
#endif
	return GSSocketUDP::WriteTo2((LPSTR)remoteAddress, remotePort, TempBuffer, PacketLength);

//	return GSSocketUDP::WriteTo(remoteAddress, remotePort, TempBuffer, PacketLength);
}

BOOL GSPacketUDP::WriteComplete(VOID)
{
	CThreadSync Sync;
#ifndef CLIENT_MODULE
	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData;
	m_WrietQueue.try_pop(pWriteData);
#endif	

	return TRUE;

}

BOOL	GSPacketUDP::ResetUdp(VOID)
{
	CThreadSync	Sync;

	m_LastReadPacketInfoVectorForUdp.clear();

	return TRUE;

}

BOOL	GSPacketUDP::GetPacket(LPSTR remoteAddress, USHORT remotePort,WORD &MainProtocol, WORD &SubProtocol,BYTE *packet, DWORD &dataLength)
{
	CThreadSync Sync;

	if (!packet)
		return FALSE;

	if (m_RemainLength < sizeof(WORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD))
		return FALSE;

	memcpy(&MainProtocol, m_PacketBuffer + sizeof(WORD), sizeof(WORD));
		
	dataLength = m_RemainLength - sizeof(WORD) - sizeof(WORD) - sizeof(WORD) - sizeof(DWORD);

	memcpy(packet, m_PacketBuffer+ sizeof(WORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), dataLength - sizeof(WORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD));
	m_RemainLength = 0;

	return TRUE;
}

}	}	}	}