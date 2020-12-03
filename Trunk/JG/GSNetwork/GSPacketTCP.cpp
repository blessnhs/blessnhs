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
#ifndef CLIENT_MODULE
	m_PacketList.clear();
#endif

	return GSSocketTCP::Termination();

}

BOOL	GSPacketTCP::ReadPacket(DWORD readLength)
{
	CThreadSync Sync;

	if (m_CurrentPacketSize <= (readLength+m_RemainLength))
	{
		DWORD Resize = readLength + m_RemainLength + m_CurrentPacketSize;
		BYTE* NewBuff = new BYTE[Resize];
		memcpy(NewBuff, m_PacketBuffer, m_CurrentPacketSize);
		delete m_PacketBuffer;

		m_PacketBuffer = NewBuff;

		m_CurrentPacketSize = Resize;

		printf("ReadPacket Resize %d\n", Resize);
	}

	if (!GSSocketTCP::ReadForIocp(m_PacketBuffer, readLength, m_RemainLength, m_CurrentPacketSize))
	{

		return FALSE;
	}

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

//BOOL    GSPacketTCP::SendPacking(LPVOID pData, DWORD dwbytes)
//{
//	char pTemp[4096] = {0,};
//	DWORD index=0;
//
//	pTemp[index++] = (BYTE)_PACKET_START1;
//	pTemp[index++] = (BYTE)_PACKET_START2;
//	memcpy( pTemp+index, &dwbytes, 2 );	// unsigned short
//	index += 2;
//	memcpy( pTemp+index, pData, dwbytes );
//	index += dwbytes;
//	pTemp[index++] = (BYTE)_PACKET_END1;
//	pTemp[index++] = (BYTE)_PACKET_END2;
//
////	printf("socket to : %d, pid : %d \n",Data->FROM, Data->MPID);
//
//	return (Send(pTemp, index));
//}
//
//BOOL	GSPacketTCP::Send(LPVOID Packet,DWORD Size)
//{
//	CThreadSync Sync;
//
//	if (!Packet)
//		return FALSE;
//	
//	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(m_WritePool.alloc());
//
//	CopyMemory(pWriteData->Data,Packet,sizeof(pWriteData->Data));
//	pWriteData->DataLength = Size;
//	pWriteData->Object = this;
//#ifndef CLIENT_MODULE	
//	m_WrietQueue.push(pWriteData);
//#endif
//	return GSSocketTCP::Write((BYTE*)pWriteData->Data, Size);
//}

BOOL	GSPacketTCP::WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE *packet, DWORD PayloadSize)
{
	CThreadSync Sync;

	if (!packet)
		return FALSE;

	DWORD PacketLength = sizeof(DWORD)  + 
		sizeof(WORD)					+ 
		sizeof(WORD)					+ 
		sizeof(DWORD)					+ 
		PayloadSize;

	byte* sendBuff = new byte[PacketLength];
	
	m_CurrentPacketNumber++;
	
	memcpy(sendBuff, &PacketLength, sizeof(DWORD));
	
	memcpy(sendBuff +
		sizeof(DWORD), 
		&MainProtocol, sizeof(WORD));
	
	memcpy(sendBuff +
		sizeof(DWORD)  + 
		sizeof(WORD), 
		&SubProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD)  + 
		sizeof(WORD)  + 
		sizeof(WORD), 
		&m_CurrentPacketNumber, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD)  + 
		sizeof(WORD)  +
		sizeof(WORD)  + 
		sizeof(DWORD),
		packet, PayloadSize);

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(/*new WRITE_PACKET_INFO*/m_WritePool.alloc());

	pWriteData->Data = sendBuff;
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

	if (m_WrietQueue.try_pop(pWriteData) == true)
	{
	}


#endif	

	

	return TRUE;
}

VOID GSPacketTCP::MakePacket(DWORD &dwPacketLength )
{
	CThreadSync Sync;

	if (ReadPacket(dwPacketLength))
	{
		while (GetPacket())
		{
			
		}
	}
}

VOID GSPacketTCP::MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength)
{
}

BOOL GSPacketTCP::GetPacket()
{
	CThreadSync Sync;

	if (m_RemainLength <= sizeof(DWORD))
		return FALSE;

	//헤더를 제외한 실제 데이터 사이즈
	DWORD PayLoadLength;

	//패킷 헤더를 포함한 전체 사이즈
	DWORD PacketLength = 0;
	memcpy(&PacketLength, m_PacketBuffer, sizeof(DWORD));

	if (PacketLength >= LMIT_BUFFER_LENGTH || PacketLength <= 0) 
	{
		printf("!GetPacket Packet Size Wrong %d\n", PacketLength);
		m_RemainLength = 0;
		return FALSE;
	}

	//2020.10.13 서버 덤프없이 사라지는 버그 패킷 전송시 클라이언트에서 Size패킷을 변조해 헤더보다 작은 값으로 보내서
	//복사하다 죽었다.
	if (PacketLength < (sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD)))
	{
		printf("!!GetPacket Packet Size Wrong %d\n", PacketLength);
		return FALSE;
	}

	if (PacketLength <= m_RemainLength)			
	{
	//	GSCrypt::Decrypt(m_PacketBuffer + sizeof(WORD), 
	//		m_PacketBuffer + sizeof(WORD), 
	//		PacketLength   - sizeof(WORD));

	
		WORD MainProtocol = 0;
		WORD SubProtocol = 0;
		DWORD SequenceNumber     = 0;

		memcpy(&MainProtocol   , m_PacketBuffer + sizeof(DWORD),   sizeof(WORD));
		memcpy(&SubProtocol    , m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD) , sizeof(WORD));
		memcpy(&SequenceNumber , m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD)  + sizeof(WORD), sizeof(DWORD));
		
		PayLoadLength = PacketLength - sizeof(DWORD) - sizeof(WORD) - sizeof(WORD) - sizeof(DWORD);

		boost::shared_ptr<XDATA> pBuffer(m_GSBufferPool.alloc());

		BYTE* Packet = pBuffer->m_Buffer.AllocBuffer(PayLoadLength);

		memcpy(Packet, m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), PayLoadLength);

		if (m_RemainLength - PacketLength > 0)
			memmove(m_PacketBuffer, m_PacketBuffer + PacketLength, m_RemainLength - PacketLength);

		m_RemainLength -= PacketLength;

		if (m_RemainLength <= 0)
		{
			m_RemainLength = 0;
			memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}
	
		pBuffer->MainId = MainProtocol;
		pBuffer->SubId = SubProtocol;
		pBuffer->Length = PayLoadLength;

		m_PacketList.push(pBuffer);

	//	MakeMsg(MainProtocol, SubProtocol, PayLoadLength);

		return TRUE;

	}

	return FALSE;
}

}	}	}	}