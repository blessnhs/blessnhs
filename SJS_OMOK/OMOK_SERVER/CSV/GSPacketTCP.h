#pragma once

#include "GSSocketTCP.h"
#include "GSPacket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketTCP	{

class GSPacketTCP : public GSNetwork::GSSocket::GSSocketTCP::GSSocketTCP, public GSNetwork::GSSocket::GSPacket::GSPacket
{
public:
	GSPacketTCP(void);
	~GSPacketTCP(void);

	BOOL	Initialize(VOID);
	BOOL	Termination(VOID);
	
	BOOL	ReadPacket(DWORD readLength);
	BOOL	ReadPacketForEventSelect(VOID);
			
	BOOL    SendPacking(LPVOID Packet,DWORD Size);
	BOOL	Send(LPVOID Packet,DWORD Size);
	BOOL	WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE *packet, WORD packetLength);

	BOOL	WriteComplete(VOID);
	BOOL	GetPacket(WORD &Mainprotocol, WORD &Subprotocol,BYTE *packet, DWORD &dataLength);

	VOID	MakePacket(DWORD dwDataLength,WORD &Mainprotocol, WORD &Subprotocol,DWORD &dataLength);

	virtual VOID MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength) {};

};

}	}	}	}