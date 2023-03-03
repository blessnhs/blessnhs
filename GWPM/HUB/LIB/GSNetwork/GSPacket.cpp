#include "StdAfx.h"
#include "GSCrypt.h"
#include "GSSocket.h"
#include "GSPacket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{

GSPacket::GSPacket(VOID)
{
	m_Max_Packet_Buffer_Size = MAX_BUFFER_LENGTH * 5;
	m_PacketBuffer = new BYTE[m_Max_Packet_Buffer_Size];

	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_RemainLength			= 0;
	m_CurrentPacketNumber	= 0;

	m_LastReadPacketNumber	= 0;

	SetSocketType(SOCKET_NONE);
}

GSPacket::~GSPacket(VOID)
{
	delete m_PacketBuffer;

	m_PacketList.clear();
	m_WrietQueue.clear();

	////종료할때 비운다.
	//boost::shared_ptr<XDATA> pBuffer;
	//while (m_PacketList.empty() == false)
	//	m_PacketList.try_pop(pBuffer);

	//boost::shared_ptr<WRITE_PACKET_INFO> pWrite;
	//while (m_WrietQueue.empty() == false)
	//	m_WrietQueue.try_pop(pWrite);

}

BOOL GSPacket::Initialize(VOID)
{
	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_RemainLength			= 0;
	m_CurrentPacketNumber	= 0;
	m_LastReadPacketNumber	= 0;

#ifndef CLIENT_MODULE
	m_WrietQueue.clear();
#endif
	return TRUE; 
}

BOOL GSPacket::Termination(VOID)
{	
#ifndef CLIENT_MODULE
	m_WrietQueue.clear();
#endif
	Initialize();

	return TRUE;
}

BYTE *GSPacket::GetPacketBuffer()
{
	return m_PacketBuffer; 
}

BYTE  GSPacket::GetSocketType()
{
	return m_SocketType;	  
}

void GSPacket::SetSocketType(BYTE _Type)
{
	m_SocketType = _Type ; 
}


GSFactory<XDATA,true> & GSPacket::GetGSBufferPool()
{
	return m_GSBufferPool;
}

}	}	}