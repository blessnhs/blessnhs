#pragma once



#include "./GSFactory.h"
#include <concurrent_queue.h>
#include "GSBuffer.h"



namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{

struct XDATA
{
	XDATA() {}
	~XDATA()
	{
	}
	XBUFFER m_Buffer;
	DWORD Length;
	WORD MainId;
	WORD SubId;
	BOOL IsCompress;

	//udp만 필요
	string LemoteAddress;
	WORD RemotePort;
};

class GSPacket 
{
public:
	GSPacket(VOID);
	virtual ~GSPacket(VOID);

	enum _SocketType
	{
		SOCKET_TCP,
		SOCKET_UDP,
		SOCKET_NONE,
	};


protected:
																			//받은 패킷을 저장하는 사이즈					
	DWORD																	m_Max_Packet_Buffer_Size;
																			//받은 패킷을 저장하는 버퍼 //패킷이 오면 큐에 넣어서꺼내기까지 몇개든지 이 버퍼에 저장					
	BYTE																	*m_PacketBuffer;
																			//처리하고 남은 사이즈 혹은 다 받지 못한 패킷사이즈

	DWORD																	m_RemainLength;

	DWORD																	m_CurrentPacketNumber;
	DWORD																	m_LastReadPacketNumber;

	BYTE																	m_SocketType;

	GSFactory<XDATA,true>													m_GSBufferPool;

	GSFactory<WRITE_PACKET_INFO,true>										m_WritePool;
	std::vector<READ_PACKET_INFO>											m_LastReadPacketInfoVectorForUdp;
	
	GSFactory<XDATA,true> &													GetGSBufferPool();
public:
#ifndef CLIENT_MODULE
	Concurrency::concurrent_queue< boost::shared_ptr<WRITE_PACKET_INFO> >			m_WrietQueue;
	Concurrency::concurrent_queue< boost::shared_ptr<XDATA> >						m_PacketList;
#endif

public:
	BOOL	Initialize(VOID);
	BOOL	Termination(VOID);

	BYTE    *GetPacketBuffer();
	BYTE    GetSocketType();
	void    SetSocketType(BYTE _Type);

};

}	}	}

typedef GSNetwork::GSSocket::GSPacket::XDATA XDATA;