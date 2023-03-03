#include "StdAfx.h"
#include "GSPacketTCP.h"
#include "GSCrypt.h"
#include "CommonSock.h"
#include "GSAllocator.h"
#include "GSMainProcess.h"
#include "MSG_PLAYER_PACKET.h"
#include "lzf.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketTCP	{

GSPacketTCP::GSPacketTCP(void)
{
	SetSocketType(SOCKET_TCP);
	m_SequenceNum = 0;
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

BOOL	GSPacketTCP::ReadPacket(DWORD ReadSize)
{
	CThreadSync Sync;

	if (m_Max_Packet_Buffer_Size <= (ReadSize +m_RemainLength))
	{
		DWORD Resize = ReadSize + m_RemainLength + m_Max_Packet_Buffer_Size;
		BYTE* NewBuff = new BYTE[Resize];
		memcpy(NewBuff, m_PacketBuffer, m_Max_Packet_Buffer_Size);
		delete m_PacketBuffer;

		m_PacketBuffer = NewBuff;

		m_Max_Packet_Buffer_Size = Resize;

		SYSLOG().Write("ReadPacket Resize %d bytes %d kbytes %d mbytes\n", Resize,Resize/1024,Resize/1024/1024);
	}

	if (!GSSocketTCP::ReadForIocp(m_PacketBuffer, ReadSize, m_RemainLength, m_Max_Packet_Buffer_Size))
	{

		return FALSE;
	}

	m_RemainLength	+= ReadSize;

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
////	SYSLOG().Write("socket to : %d, pid : %d \n",Data->FROM, Data->MPID);
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
// 
// 
// 
// 

//2022 06 24 extend protocol header
// size 4 + main protocol id 2 + sub protocol id 2 + sequence 4 + reserve1 1 + reserver2 8
//

BOOL	GSPacketTCP::WritePacketNoneCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize, long long Reserve2)
{
	CThreadSync Sync;

	if (!Packet)
		return FALSE;

	DWORD PacketLength = 
		sizeof(DWORD) +			// size 4
		sizeof(WORD) +			// main protocol id 2
		sizeof(WORD) +			// sub protocol id 2
		sizeof(DWORD) +			// sequence 4
		sizeof(byte) +			// reserve1 1
		sizeof(long long) +		//reserver2 8
		PayloadSize;

	byte* sendBuff = new byte[PacketLength];

	byte compressflag = 0;

	memcpy(sendBuff, &PacketLength, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD),
		&MainProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD),
		&SubProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD),
		&m_SequenceNum, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) + 
		sizeof(DWORD),
		&compressflag, sizeof(byte));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) + 
		sizeof(byte),
		&Reserve2, sizeof(long long));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) + 
		sizeof(byte) + 
		sizeof(long long),
		Packet, PayloadSize);

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(new WRITE_PACKET_INFO);

	pWriteData->Data = sendBuff;
	pWriteData->DataLength = PacketLength;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif	

	//return GSSocketTCP::Write((BYTE*)pWriteData->Data, pWriteData->DataLength);
	return GSSocketTCP::Write(pWriteData);
}

BOOL	GSPacketTCP::RelayPacket(WORD MainProtocol, WORD SubProtocol, BOOL Compress, const BYTE* Packet, DWORD PayloadSize, long long Reserve2)
{

	CThreadSync Sync;

	if (!Packet)
		return FALSE;

	DWORD PacketLength =
		sizeof(DWORD) +			// size 4
		sizeof(WORD) +			// main protocol id 2
		sizeof(WORD) +			// sub protocol id 2
		sizeof(DWORD) +			// sequence 4
		sizeof(byte) +			// reserve1 1
		sizeof(long long) +		//reserver2 8
		PayloadSize;

	byte* sendBuff = new byte[PacketLength];

	byte compressflag = Compress;

	memcpy(sendBuff, &PacketLength, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD),
		&MainProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD),
		&SubProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD),
		&m_SequenceNum, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD),
		&compressflag, sizeof(byte));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		sizeof(byte),
		&Reserve2, sizeof(long long));


	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		sizeof(byte) +
		sizeof(long long),
		Packet, PayloadSize);

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(new WRITE_PACKET_INFO);

	pWriteData->Data = sendBuff;
	pWriteData->DataLength = PacketLength;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif	

	//return GSSocketTCP::Write((BYTE*)pWriteData->Data, pWriteData->DataLength);
	return GSSocketTCP::Write(pWriteData);
}


BOOL	GSPacketTCP::WritePacketCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize, long long Reserve2)
{
	CThreadSync Sync;

	if (!Packet)
		return FALSE;

	//compress

	std::string src((char*)Packet, PayloadSize);

	std::string out = Gzip::compress(src);
	
	DWORD PacketLength =
		sizeof(DWORD) +			// size 4
		sizeof(WORD) +			// main protocol id 2
		sizeof(WORD) +			// sub protocol id 2
		sizeof(DWORD) +			// sequence 4
		sizeof(byte) +			// reserve1 1
		sizeof(long long) +		//reserver2 8
		out.size();

	byte* sendBuff = new byte[PacketLength];

	byte compressflag = 1;

	memcpy(sendBuff, &PacketLength, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD),
		&MainProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD),
		&SubProtocol, sizeof(WORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD),
		&m_SequenceNum, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD),
		&compressflag, sizeof(byte));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		sizeof(byte),
		&Reserve2, sizeof(long long));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		sizeof(byte) +
		sizeof(long long),
		out.c_str(), out.size());

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));


	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(new WRITE_PACKET_INFO);

	pWriteData->Data = sendBuff;
	pWriteData->DataLength = PacketLength;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif	

	return GSSocketTCP::Write(pWriteData);
}

BOOL	GSPacketTCP::WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE * Packet, DWORD PayloadSize, long long Reserve2)
{
	if (PayloadSize > 760)
		return WritePacketCompress(MainProtocol, SubProtocol, Packet, PayloadSize, Reserve2);
	else
		return WritePacketNoneCompress(MainProtocol, SubProtocol, Packet, PayloadSize, Reserve2);

}

BOOL GSPacketTCP::WriteComplete(VOID)
{
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
		SYSLOG().Write("!GetPacket Packet Size Wrong %d\n", PacketLength);
		m_RemainLength = 0;
		return FALSE;
	}

	//2020.10.13 서버 덤프없이 사라지는 버그 패킷 전송시 클라이언트에서 Size패킷을 변조해 헤더보다 작은 값으로 보내서
	//복사하다 죽었다.
	//4바이트 (크기) + 2바이트(protocolid)  + 2바이트 (sub protocolid) + 4바이트(sequence number) + 1바이트(압축여부) reserver1 + 8바이트 reserver2
	if (PacketLength < (sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(byte) + sizeof(long long) ))
	{
		SYSLOG().Write("!!GetPacket Packet Size Wrong %d\n", PacketLength);
		return FALSE;
	}

	if (PacketLength <= m_RemainLength)			
	{
	//	GSCrypt::Decrypt(m_PacketBuffer + sizeof(WORD), 
	//		m_PacketBuffer + sizeof(WORD), 
	//		PacketLength   - sizeof(WORD));

	
		WORD MainProtocol = 0;
		WORD SubProtocol = 0;
		DWORD SequenceNumber = 0;
		byte CompressFlag     = 0;

		long long Reserve2;

		memcpy(&MainProtocol   , m_PacketBuffer + sizeof(DWORD),   sizeof(WORD));
		memcpy(&SubProtocol    , m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD) , sizeof(WORD));
		memcpy(&SequenceNumber, m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD)  + sizeof(WORD), sizeof(DWORD));

		memcpy(&CompressFlag, m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), sizeof(byte));
		memcpy(&Reserve2, m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(byte), sizeof(long long));
		
		PayLoadLength = PacketLength - sizeof(DWORD) - sizeof(WORD) - sizeof(WORD) - sizeof(DWORD) - sizeof(byte) - sizeof(long long);

		boost::shared_ptr<XDATA> pBuffer(m_GSBufferPool.alloc());

		if(CompressFlag == 1 && m_UseCompress == true)
		{
			std::string src((char *)(m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(byte) + sizeof(long long)), PayLoadLength);

			std::string out = Gzip::decompress(src);

			pBuffer->m_Buffer.SetBuffer((BYTE *)out.c_str(), out.size());
		}
		else
		{
			pBuffer->m_Buffer.SetBuffer(m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(byte) + sizeof(long long), PayLoadLength);
		}
	
		pBuffer->IsCompress = CompressFlag;
		pBuffer->MainId = MainProtocol;
		pBuffer->SubId = SubProtocol;
		pBuffer->Reserve2 = Reserve2;
		pBuffer->Length = pBuffer->m_Buffer.GetLength();

		m_PacketList.push(pBuffer);


		if (m_RemainLength - PacketLength > 0)
			memmove(m_PacketBuffer, m_PacketBuffer + PacketLength, m_RemainLength - PacketLength);

		m_RemainLength -= PacketLength;

		if (m_RemainLength <= 0)
		{
			m_RemainLength = 0;
			memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}

	//	MakeMsg(MainProtocol, SubProtocol, PayLoadLength);

		return TRUE;

	}

	return FALSE;
}

}	}	}	}