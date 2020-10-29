#pragma once

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>

#include "./GSFactory.h"
#ifndef CLIENT_MODULE
#include <concurrent_queue.h>
#endif
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

	//udp¸¸ ÇÊ¿ä
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
	BYTE																	m_PacketBuffer[MAX_BUFFER_LENGTH * 5];
	INT																		m_RemainLength;

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