#include "StdAfx.h"
#include "GSCrypt.h"
#include "GSSocket.h"
#include "GSPacket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{

GSPacket::GSPacket(VOID)
{
	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_RemainLength			= 0;
	m_CurrentPacketNumber	= 0;

	m_LastReadPacketNumber	= 0;

	SetSocketType(SOCKET_NONE);
}

GSPacket::~GSPacket(VOID)
{
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