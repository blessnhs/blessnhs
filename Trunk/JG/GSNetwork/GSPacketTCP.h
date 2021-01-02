#pragma once

#include "GSSocketTCP.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSPacket	{	namespace GSPacketTCP	{

class GSPacketTCP : public GSNetwork::GSSocket::GSSocketTCP::GSSocketTCP, public GSNetwork::GSSocket::GSPacket::GSPacket
{
public:
	GSPacketTCP(void);
	virtual ~GSPacketTCP(void);

	BOOL	Initialize(VOID);
	BOOL	Termination(VOID);
	
	BOOL	ReadPacket(DWORD readLength);
	BOOL	ReadPacketForEventSelect();
			
	//BOOL    SendPacking(LPVOID Packet,DWORD Size);
	//BOOL	Send(LPVOID Packet,DWORD Size);
	BOOL	WritePacket(WORD MainProtocol,WORD SubProtocol, const BYTE *Packet, DWORD PayloadSize);

	BOOL	WritePacketCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize);
	BOOL	WritePacketNoneCompress(WORD MainProtocol, WORD SubProtocol, const BYTE* Packet, DWORD PayloadSize);

	BOOL	WriteComplete(VOID);
	BOOL	GetPacket();

	VOID	MakePacket(DWORD &dataLength);

	virtual VOID MakeMsg(WORD Mainprotocol, WORD Subprotocol,DWORD dataLength);

};

}	}	}	}

#define GSPACKETTCP GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP 