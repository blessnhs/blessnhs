#include "StdAfx.h"
#include "CommonSock.h"

void CCommonSock::ReceivedData(char* pData, DWORD dwbytes)
{
	m_CommonPacketBuffer.PutData(pData, dwbytes);

	bool retval = false;
	do {
		DWORD dwGetLen=0;
		int send_index=0, index=0;
		char tempbuff[_PACKET_MTU] = {0,};
		char pGetBuff[_PACKET_MTU] = {0,};
		retval = PullOutCore(pGetBuff, dwGetLen);

		if(retval) {

			Process((LPVOID)pGetBuff);
		}
	}while(retval);
}

bool CCommonSock::PullOutCore(char* pGetBuff, DWORD &dwGetBytes)
{
	DWORD			DataLen=0;
	bool			foundCore=false;
	unsigned short	slen=0;

	DataLen = m_CommonPacketBuffer.GetValidDataLength();
	if(DataLen <= 4) return false;							// START1, START2, END1, END2
	BYTE pTempBuff[_PACKET_MTU] = {0,};
	if(DataLen>sizeof(pTempBuff))
		DataLen = sizeof(pTempBuff);
	m_CommonPacketBuffer.GetData((char*)pTempBuff, DataLen);

	DWORD	sPos=0, ePos=0;									// start position and end position...
	for(register DWORD i=0; i<DataLen; i++) {
		if(pTempBuff[i] == _PACKET_START1 && pTempBuff[i+1] == _PACKET_START2) {
//			if(i!=0)	m_PacketBuffer.MoveHeadPos(i);		// set current position for valid position
			sPos = i+2;										// after PACKET_START1+PACKET_START2
			memcpy(&slen, &pTempBuff[sPos], 2);				// length data
			if(slen>m_dwReadSize) {
				m_CommonPacketBuffer.MoveHeadPos(2);				// wrong size
				return false;
			}
			else if(slen>DataLen)							// this packet don`t have received yet...
				return false;
			ePos = sPos+slen +2;							// end parity checking position ( 2 : length size )
			if( (ePos+2)>DataLen )							// 2 : PACKET_END1 + PACKET_END2
				return false;								// this packet don`t have received yet...
			if(pTempBuff[ePos] == _PACKET_END1 && pTempBuff[ePos+1] == _PACKET_END2) {
				dwGetBytes = slen;							// real packet size
				memcpy(pGetBuff, (const void*)(pTempBuff+sPos+2), dwGetBytes);
				foundCore = true;
				m_CommonPacketBuffer.MoveHeadPos(6+dwGetBytes);	// 6bytes : header 2bytes + end 2bytes + length 2bytes
				break;
			}
			else {											// Packet Wrapping Error!!
				m_CommonPacketBuffer.MoveHeadPos(2);				// refind _FROM next position...
				break;
			}
		}
		else {
			m_CommonPacketBuffer.MoveHeadPos(1);
		}
	}
	return foundCore;
}
