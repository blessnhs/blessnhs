#pragma once

#include "GSEventSelect.h"

#include "GSPacketTCP.h"
#include "GSPacketUDP.h"

namespace GSNetwork { namespace GSEventSelect {	namespace GSClientModule	{

typedef GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;
typedef GSNetwork::GSSocket::GSPacket::GSPacketUDP::GSPacketUDP GSPacketUDP;

class GSClientModule : public GSEventSelect
{
public:
	GSClientModule(VOID);
	virtual ~GSClientModule(VOID);

private:
	GSPacketTCP	mTCPSession;
	GSPacketUDP	mUDPSession;


	std::queue<std::shared_ptr<READ_PACKET_INFO>>		m_ReadPacketQueue;					

	BOOL			IsUdp;

public:
	BOOL			BeginTcp(LPSTR remoteAddress, USHORT remotePort);
	BOOL			BeginUdp(USHORT remotePort);

	BOOL			End(VOID);

	BOOL			GetLocalIP(string pIP);
	USHORT			GetLocalPort(VOID);

	BOOL			ReadPacket(WORD &protocol, BYTE *packet, WORD &packetLength);

	BOOL			WritePacket(WORD protocol, WORD Subprotocol,const BYTE *packet, WORD packetLength);

protected:
	VOID			OnIoRead(VOID);
};

}	}	}