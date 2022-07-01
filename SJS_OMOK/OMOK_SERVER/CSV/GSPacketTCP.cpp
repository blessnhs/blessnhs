#include "StdAfx.h"
#include "GSPacketTCP.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketTCP	{

GSPacketTCP::GSPacketTCP(void)
{
}

GSPacketTCP::~GSPacketTCP(void)
{
}

BOOL	GSPacketTCP::Initialize(VOID)
{
	GSPacket::Initialize();

	return GSSocketTCP::Initialize();
}

BOOL	GSPacketTCP::Termination(VOID)
{

	GSPacket::Termination();
	SetSocketStatus(STATUS_DISCONNECTED);

	return GSSocketTCP::Termination();

}

BOOL	GSPacketTCP::ReadPacket(DWORD readLength)
{
	if (!GSSocketTCP::ReadForIocp(m_PacketBuffer, readLength,m_RemainLength))
		return FALSE;

	m_RemainLength	+= readLength;

	//return getPacket(protocol, packet, packetLength);
	return TRUE;
}

BOOL	GSPacketTCP::ReadPacketForEventSelect()
{
	DWORD ReadLength = 0;

	if (!GSSocketTCP::ReadForEventSelect(m_PacketBuffer + m_RemainLength, ReadLength))
		return FALSE;

	m_RemainLength	+= ReadLength;

	//return getPacket(protocol, packet, packetLength);
	return TRUE;
}

BOOL	GSPacketTCP::Send(LPVOID Packet,DWORD Size)
{
	if (!Packet)
		return FALSE;

	return GSSocketTCP::Write((BYTE*)Packet, Size);
}

BOOL	GSPacketTCP::WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE *packet, WORD packetLength)
{

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

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	return GSSocketTCP::Write((BYTE*)TempBuffer, PacketLength);
}

BOOL GSPacketTCP::WriteComplete(VOID)
{
	return TRUE;
}

VOID GSPacketTCP::MakePacket(DWORD dwDataLength,WORD &Mainprotocol, WORD &Subprotocol,DWORD &dataLength)
{
	WORD  MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;
	BYTE Packet[MAX_BUFFER_LENGTH] = {0,};

	if (ReadPacket(dwDataLength))
	{
		while (GetPacket(MainProtocol,SubProtocol, Packet, dwPacketLength))
		{
			MakeMsg(MainProtocol,SubProtocol,dwPacketLength);
		}
	}
}

BOOL GSPacketTCP::GetPacket(WORD &MainProtocol, WORD &SubProtocol,BYTE *packet, DWORD &dataLength)
{
	if (!packet)
		return FALSE;

	if (m_RemainLength <= sizeof(WORD))
		return FALSE;

	WORD PacketLength = 0;
	memcpy(&PacketLength, m_PacketBuffer, sizeof(WORD));

	if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0) 
	{
		m_RemainLength = 0;
		return FALSE;
	}

	if (PacketLength <= m_RemainLength)			
	{
	//	GSCrypt::Decrypt(m_PacketBuffer + sizeof(WORD), 
	//		m_PacketBuffer + sizeof(WORD), 
	//		PacketLength   - sizeof(WORD));

		
		
		DWORD SequenceNumber     = 0;
		WORD From;

		memcpy(&MainProtocol   , m_PacketBuffer + sizeof(WORD),   sizeof(WORD));
		memcpy(&SubProtocol    , m_PacketBuffer + sizeof(WORD)  + sizeof(WORD) , sizeof(WORD));
		memcpy(&SequenceNumber , m_PacketBuffer + sizeof(WORD)  + sizeof(WORD)  + sizeof(WORD), sizeof(DWORD));
		
		dataLength = PacketLength - sizeof(WORD) - sizeof(WORD) - sizeof(WORD) - sizeof(DWORD);
	
		memcpy(packet, m_PacketBuffer + sizeof(WORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), dataLength);

		if (m_RemainLength - PacketLength > 0)
			memmove(m_PacketBuffer, m_PacketBuffer + PacketLength, m_RemainLength - PacketLength);

		m_RemainLength -= PacketLength;

		if (m_RemainLength <= 0)
		{
			m_RemainLength = 0;
			//memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}
		return TRUE;

	}

	return FALSE;
}

}	}	}	}