#pragma once

#include "PacketBuffer.h"


// packet wrap parity bytes
const BYTE _PACKET_START1		= 0x55;
const BYTE _PACKET_START2		= 0xBB;
const BYTE _PACKET_END1			= 0x55;
const BYTE _PACKET_END2			= 0xBB;

#define _PACKET_MTU					4096	// Max Packet Size...

class CSockAdmin;
class CCommonSock
{
public:
	CCommonSock() {m_pAdmin=NULL;m_dwReadSize=8192;};
	virtual ~CCommonSock() {};
	
	VOID SetType(BYTE Type) { m_dwType = Type; }
	BYTE GetType() { return m_dwType; }

	void ReceivedData(char* pData, DWORD dwbytes);
	bool PullOutCore(char* pGetBuff, DWORD &dwGetBytes);
	virtual VOID Process(LPVOID pGetBuff){};

	WORD			m_wSid;
	BYTE			m_dwType;
	GSPacketBuffer	m_CommonPacketBuffer;
	DWORD			m_dwReadSize;
	CSockAdmin*		m_pAdmin;
};