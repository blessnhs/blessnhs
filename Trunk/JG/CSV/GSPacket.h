#pragma once

#include "GSSocketBase.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{

class GSPacket 
{
public:
	GSPacket(VOID);
	virtual ~GSPacket(VOID);

protected:
	BYTE																	m_PacketBuffer[MAX_BUFFER_LENGTH * 3];
	INT																		m_RemainLength;
	DWORD																	m_CurrentPacketNumber;
	
	DWORD																	m_LastReadPacketNumber;

	std::vector<READ_PACKET_INFO>											m_LastReadPacketInfoVectorForUdp;
	

public:
	BOOL	Initialize(VOID);
	BOOL	Termination(VOID);
};

}	}	}
