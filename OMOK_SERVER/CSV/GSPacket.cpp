#include "StdAfx.h"

#include "GSSocket.h"
#include "GSPacket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{

GSPacket::GSPacket(VOID)
{
	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_RemainLength			= 0;
	m_CurrentPacketNumber	= 0;

	m_LastReadPacketNumber	= 0;
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

	return TRUE; 
}

BOOL GSPacket::Termination(VOID)
{
	Initialize();

	return TRUE;
}

}	}	}