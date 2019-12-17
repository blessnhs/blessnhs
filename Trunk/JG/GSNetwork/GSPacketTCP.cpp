#include "StdAfx.h"
#include "GSPacketTCP.h"
#include "GSCrypt.h"
#include "CommonSock.h"
#include "GSAllocator.h"
#include "GSMainProcess.h"
#include "MSG_PLAYER_PACKET.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketTCP	{

GSPacketTCP::GSPacketTCP(void)
{
	SetSocketType(SOCKET_TCP);
}

GSPacketTCP::~GSPacketTCP(void)
{
}

BOOL	GSPacketTCP::Initialize(VOID)
{
	CThreadSync Sync;

	GSPacket::Initialize();

	return GSSocketTCP::Initialize();
}

BOOL	GSPacketTCP::Termination(VOID)
{
		
	CThreadSync Sync;

	GSPacket::Termination();
	SetSocketStatus(STATUS_DISCONNECTED);
#ifndef CLIENT_MODULE
	m_PacketList.clear();
#endif
	return GSSocketTCP::Termination();

}

BOOL	GSPacketTCP::ReadPacket(DWORD readLength)
{
	CThreadSync Sync;

	if (!GSSocketTCP::ReadForIocp(m_PacketBuffer, readLength,m_RemainLength))
		return FALSE;

	m_RemainLength	+= readLength;

	//return getPacket(protocol, packet, packetLength);
	return TRUE;
}

BOOL	GSPacketTCP::ReadPacketForEventSelect()
{
	CThreadSync Sync;

	DWORD ReadLength = 0;

	if (!GSSocketTCP::ReadForEventSelect(m_PacketBuffer + m_RemainLength, ReadLength))
		return FALSE;

	m_RemainLength	+= ReadLength;

	//return getPacket(protocol, packet, packetLength);
	return TRUE;
}

BOOL    GSPacketTCP::SendPacking(LPVOID pData, DWORD dwbytes)
{
	char pTemp[4096] = {0,};
	DWORD index=0;

	pTemp[index++] = (BYTE)_PACKET_START1;
	pTemp[index++] = (BYTE)_PACKET_START2;
	memcpy( pTemp+index, &dwbytes, 2 );	// unsigned short
	index += 2;
	memcpy( pTemp+index, pData, dwbytes );
	index += dwbytes;
	pTemp[index++] = (BYTE)_PACKET_END1;
	pTemp[index++] = (BYTE)_PACKET_END2;

//	printf("socket to : %d, pid : %d \n",Data->FROM, Data->MPID);

	return (Send(pTemp, index));
}

BOOL	GSPacketTCP::Send(LPVOID Packet,DWORD Size)
{
	CThreadSync Sync;

	if (!Packet)
		return FALSE;
	
	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(m_WritePool.alloc());

	CopyMemory(pWriteData->Data,Packet,sizeof(pWriteData->Data));
	pWriteData->DataLength = Size;
	pWriteData->Object = this;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif
	return GSSocketTCP::Write((BYTE*)pWriteData->Data, Size);
}

BOOL	GSPacketTCP::WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE *packet, WORD packetLength)
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

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(/*new WRITE_PACKET_INFO*/m_WritePool.alloc());

	CopyMemory(pWriteData->Data,TempBuffer,sizeof(pWriteData->Data));
	pWriteData->DataLength = PacketLength;
	pWriteData->Object = this;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData); 
#endif	

	return GSSocketTCP::Write((BYTE*)pWriteData->Data, PacketLength);
}

BOOL GSPacketTCP::WriteComplete(VOID)
{
	CThreadSync Sync;

#ifndef CLIENT_MODULE
	if(m_WrietQueue.unsafe_size() == 0) return FALSE;

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData;

	m_WrietQueue.try_pop(pWriteData);
#endif	
	return TRUE;
}

VOID GSPacketTCP::MakePacket(DWORD dwDataLength,WORD &Mainprotocol, WORD &Subprotocol,DWORD &dwPacketLength )
{
	CThreadSync Sync;
	
	WORD  MainProtocol = 0,SubProtocol = 0;
	BYTE Packet[MAX_BUFFER_LENGTH] = {0,};
	memset(Packet,0,MAX_BUFFER_LENGTH);

	if (ReadPacket(dwDataLength))
	{
		while (GetPacket(MainProtocol,SubProtocol, Packet, dwPacketLength))
		{
			boost::shared_ptr<XDATA> pBuffer( m_GSBufferPool.alloc());
			pBuffer->m_Buffer.SetBuffer(Packet,dwPacketLength);
			pBuffer->MainId = MainProtocol;
			pBuffer->SubId  = Subprotocol;
			pBuffer->Length = dwPacketLength ;

			m_PacketList.push(pBuffer);

			MakeMsg(MainProtocol,SubProtocol,dwPacketLength);
		}
	}
}

VOID GSPacketTCP::MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength)
{
}

BOOL GSPacketTCP::GetPacket(WORD &MainProtocol, WORD &SubProtocol,BYTE *packet, DWORD &dataLength)
{
	CThreadSync Sync;

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
			memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}
		return TRUE;

	}

	return FALSE;
}

}	}	}	}