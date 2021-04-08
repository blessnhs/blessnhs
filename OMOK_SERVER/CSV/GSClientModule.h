#pragma once

#include "GSPacketTCP.h"
#include <queue>
#include "GSEventSelect.h"

extern DWORD MyIndex;

class GSClientModule : public GSEventSelect
{
public:
	GSClientModule(VOID);
	virtual ~GSClientModule(VOID);

private:
	GSNetwork::GSSocket::GSPacket::GSPacketTCP::GSPacketTCP	mTCPSession;

	BOOL			IsUdp;

public:

	std::queue<std::shared_ptr<READ_PACKET_INFO>>		m_ReadPacketQueue;					

	BOOL			BeginTcp(LPSTR remoteAddress, USHORT remotePort);
	BOOL			BeginUdp(USHORT remotePort);

	BOOL			End(VOID);

	BOOL			GetLocalIP(WCHAR* pIP);
	USHORT			GetLocalPort(VOID);

	BOOL			ReadPacket(WORD &protocol, BYTE *packet, WORD &packetLength);

	BOOL			WritePacket(WORD protocol, WORD Subprotocol,const BYTE *packet, WORD packetLength);


	BOOL			IsConnected;

	template<class T>
	void Send(T &C)
	{
		std::stringstream m_Stream; 
		boost::archive::text_oarchive oa(m_Stream);

		oa << C;

		HEADER hdr = (HEADER)C;

		WritePacket(hdr.MainId,hdr.SubId,(BYTE*)((string)m_Stream.str()).data(),m_Stream.str().size());
	}

protected:
	VOID			OnIoRead(VOID);

	virtual VOID	OnIoConnected(VOID)		{  IsConnected  = true; };
	virtual VOID	OnIoDisconnected(VOID)	{};

public:
	VOID Proc();
};

