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

	if (m_CurrentPacketSize <= (ReadSize +m_RemainLength))
	{
		DWORD Resize = ReadSize + m_RemainLength + m_CurrentPacketSize;
		BYTE* NewBuff = new BYTE[Resize];
		memcpy(NewBuff, m_PacketBuffer, m_CurrentPacketSize);
		delete m_PacketBuffer;

		m_PacketBuffer = NewBuff;

		m_CurrentPacketSize = Resize;

		SYSLOG().Write("ReadPacket Resize %d bytes %d kbytes %d mbytes\n", Resize,Resize/1024,Resize/1024/1024);
	}

	if (!GSSocketTCP::ReadForIocp(m_PacketBuffer, ReadSize, m_RemainLength, m_CurrentPacketSize))
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

BOOL	GSPacketTCP::WritePacketNoneCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize)
{

	CThreadSync Sync;

	if (!Packet)
		return FALSE;

	DWORD PacketLength = sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		PayloadSize;

	byte* sendBuff = new byte[PacketLength];

	int compressflag = 0;

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
		&compressflag, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD),
		Packet, PayloadSize);

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(/*new WRITE_PACKET_INFO*/m_WritePool.alloc());

	pWriteData->Data = sendBuff;
	pWriteData->DataLength = PacketLength;
	pWriteData->Object = this;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif	

	return GSSocketTCP::Write((BYTE*)pWriteData->Data, pWriteData->DataLength);
}

BOOL	GSPacketTCP::WritePacketCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize)
{
	CThreadSync Sync;

	if (!Packet)
		return FALSE;

	//compress

	int lzf_compress_size = PayloadSize + (PayloadSize / 2); //더 늘어날수도 있다.
	byte* lzf_compress_buffer = new byte[lzf_compress_size];

	int out_lzf_size = lzf_compress(Packet, PayloadSize, lzf_compress_buffer, lzf_compress_size);

	if (out_lzf_size == 0)
	{
		SYSLOG().Write("lzf_compress failed src size %d alloc size %d dst size %d mid %d payloadsize %d\n", PayloadSize, lzf_compress_size, out_lzf_size, MainProtocol, PayloadSize);

		delete lzf_compress_buffer;
		return false;
	}
	//


	DWORD PacketLength = sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD) +
		out_lzf_size;

	byte* sendBuff = new byte[PacketLength];

	int compressflag = 1;

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
		&compressflag, sizeof(DWORD));

	memcpy(sendBuff +
		sizeof(DWORD) +
		sizeof(WORD) +
		sizeof(WORD) +
		sizeof(DWORD),
		lzf_compress_buffer, out_lzf_size);

	//GSCrypt::Encrypt(TempBuffer + sizeof(WORD), TempBuffer + sizeof(WORD), PacketLength - sizeof(WORD));

	//
	delete lzf_compress_buffer;

	boost::shared_ptr<WRITE_PACKET_INFO> pWriteData(/*new WRITE_PACKET_INFO*/m_WritePool.alloc());

	pWriteData->Data = sendBuff;
	pWriteData->DataLength = PacketLength;
	pWriteData->Object = this;
#ifndef CLIENT_MODULE	
	m_WrietQueue.push(pWriteData);
#endif	

	return GSSocketTCP::Write((BYTE*)pWriteData->Data, pWriteData->DataLength);
}

BOOL	GSPacketTCP::WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE * Packet, DWORD PayloadSize)
{
	if (PayloadSize > 760)
		return WritePacketCompress(MainProtocol, SubProtocol, Packet, PayloadSize);
	else
		return WritePacketNoneCompress(MainProtocol, SubProtocol, Packet, PayloadSize);

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
		SYSLOG().Write("!GetPacket Packet Size Wrong %d\n", PacketLength);
		m_RemainLength = 0;
		return FALSE;
	}

	//2020.10.13 서버 덤프없이 사라지는 버그 패킷 전송시 클라이언트에서 Size패킷을 변조해 헤더보다 작은 값으로 보내서
	//복사하다 죽었다.
	if (PacketLength < (sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD)))
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
		DWORD CompressFlag     = 0;

		memcpy(&MainProtocol   , m_PacketBuffer + sizeof(DWORD),   sizeof(WORD));
		memcpy(&SubProtocol    , m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD) , sizeof(WORD));
		memcpy(&CompressFlag, m_PacketBuffer + sizeof(DWORD)  + sizeof(WORD)  + sizeof(WORD), sizeof(DWORD));
		
		PayLoadLength = PacketLength - sizeof(DWORD) - sizeof(WORD) - sizeof(WORD) - sizeof(DWORD);

		boost::shared_ptr<XDATA> pBuffer(m_GSBufferPool.alloc());

		if(CompressFlag == 1)
		{

			//압축해제 사이즈가 부족할수 있다. 최대 20번까지 시도
			int decompress_buffer_size = PayLoadLength * 2;
			byte* decompress_buff = new byte[decompress_buffer_size];
			int decompress_out_size = 0;
			bool success = false;
			for (int i = 0; i < 20; i++)
			{
				decompress_out_size = lzf_decompress(m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), 
					PayLoadLength, decompress_buff, decompress_buffer_size);
				if (decompress_out_size != 0)
				{
					//SYSLOG().Write(" src size %d dst size %d\n", PayLoadLength, decompress_out_size);
					success = true;
					break;
				}

				delete decompress_buff;
				decompress_buffer_size += decompress_buffer_size;

				decompress_buff = new byte[decompress_buffer_size];
			}

			if (success == false)
			{
				SYSLOG().Write("decompress error mid %d payloadsize %d\n", MainProtocol, PacketLength);
				delete decompress_buff;
				return -1;
			}

			pBuffer->m_Buffer.SetBuffer(decompress_buff, decompress_out_size);

			delete decompress_buff;
		}
		else
		{
			pBuffer->m_Buffer.SetBuffer(m_PacketBuffer + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) + sizeof(DWORD), PayLoadLength);
		}
	
		pBuffer->MainId = MainProtocol;
		pBuffer->SubId = SubProtocol;
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