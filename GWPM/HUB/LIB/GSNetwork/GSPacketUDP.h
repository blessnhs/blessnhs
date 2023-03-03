#pragma once

#include "GSSocketUDP.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketUDP	{

class GSPacketUDP : public GSNetwork::GSSocket::GSSocketUDP::GSSocketUDP , public GSNetwork::GSSocket::GSPacket::GSPacket
{
public:
	GSPacketUDP(void);
	virtual ~GSPacketUDP(void);

	BOOL	Initialize(VOID);
	BOOL	Termination(VOID);

	BOOL	ReadFromPacketForIocp(LPSTR& remoteAddress, USHORT &remotePort, DWORD readLength);
	BOOL	ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT &remotePort);

	BOOL	WriteToPacket(LPCSTR remoteAddress, USHORT remotePort, WORD MainProtocol, WORD SubProtocol,const BYTE *packet, WORD packetLength);
	BOOL	WriteComplete(VOID);

	VOID	MakePacket(DWORD dwDataLength);
	virtual VOID MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength) {};

	BOOL	ResetUdp(VOID);
	BOOL	GetPacket(LPSTR remoteAddress, USHORT remotePort, WORD &MainProtocol, WORD &SubProtocol,BYTE *packet, DWORD &dataLength);

};

}	}	}	}