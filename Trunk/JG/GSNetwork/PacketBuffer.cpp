#include "StdAfx.h"
#include "PacketBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////
// CPacketBuffer Implementation...
////////////////////////////////////////////////////////////////////////////////////////
GSPacketBuffer::GSPacketBuffer(DWORD dwBufferSize)
{
	m_dwHeadPos=0;
	m_dwTailPos=0;
	m_dwBufferSize=dwBufferSize;
	m_pBuffer = new char[m_dwBufferSize];
}

GSPacketBuffer::~GSPacketBuffer()
{
	if(m_pBuffer != NULL) delete m_pBuffer;
}

bool GSPacketBuffer::PutData(char* pData, DWORD length)
{
    if( !pData || (length>m_dwBufferSize) ) 
		return false;

		while(IsOverflow(length))
			ReSize();
		if(m_dwTailPos+length >= m_dwBufferSize) {
			DWORD dwFirstBytes = m_dwBufferSize - m_dwTailPos;
			DWORD dwSecondBytes = length - dwFirstBytes;
			memcpy( m_pBuffer+m_dwTailPos, pData, dwFirstBytes );
			if(dwSecondBytes>0) {
				memcpy(m_pBuffer, pData+dwFirstBytes, dwSecondBytes);
				m_dwTailPos = dwSecondBytes;
			}
			else m_dwTailPos = 0;
		}
		else
		{
			memcpy(m_pBuffer+m_dwTailPos, pData, length);
			m_dwTailPos += length;
		}
	return true;
}

bool GSPacketBuffer::GetData(char* pData, DWORD length)
{
    if( !pData || (length>GetValidDataLength()) ) 
		return false;
		DWORD dwSize = m_dwBufferSize-m_dwHeadPos;
		if(length <= dwSize) 
			memcpy(pData, m_pBuffer+m_dwHeadPos, length);
		else {
			memcpy(pData, m_pBuffer+m_dwHeadPos, dwSize);
			memcpy(pData+dwSize, m_pBuffer, length-dwSize);
		}
	return true;
}

DWORD GSPacketBuffer::GetValidDataLength()
{
    int nDataLength = m_dwTailPos-m_dwHeadPos;
    if(nDataLength<0) nDataLength = m_dwBufferSize+nDataLength;
    return (DWORD)nDataLength;
}

void GSPacketBuffer::MoveHeadPos(DWORD length)
{
    if(length > GetValidDataLength()) 
		length = GetValidDataLength();
    m_dwHeadPos += length; 
	m_dwHeadPos %= m_dwBufferSize;
}

void GSPacketBuffer::ReSize()
{
		DWORD prevBufSize = m_dwBufferSize;
		m_dwBufferSize <<= 1;
		char* pNewData = new char[m_dwBufferSize];
		memcpy(pNewData, m_pBuffer, prevBufSize);
		if(m_dwTailPos < m_dwHeadPos) {
			memcpy(pNewData+prevBufSize, m_pBuffer, m_dwTailPos );
			m_dwTailPos += prevBufSize;
		}
		delete m_pBuffer;
		m_pBuffer = pNewData;
}

bool GSPacketBuffer::IsOverflow(DWORD length)
{
    return ( length >= (m_dwBufferSize-GetValidDataLength()) );
}
