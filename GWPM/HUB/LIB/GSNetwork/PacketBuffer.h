#pragma once



class GSPacketBuffer
{
public:
	GSPacketBuffer(DWORD dwBufferSize=16384);
	~GSPacketBuffer();
	 
	 bool PutData(char* pData, DWORD length);	// Input to BufferQueue Received Data from Socket
	 bool GetData(char* pData, DWORD length);	// Output Data from BufferQueue. length is requesting data size
	 DWORD GetValidDataLength();				// Get Length of Valid Data from BufferQueue 
	 void MoveHeadPos(DWORD length);			// Data Current Head Pointer move an extent of length (for Data Delete)
	 void ReSize();							// Resize BufferQueue for BufferQueue overflow
	 void SetInit() { m_dwHeadPos=0; m_dwTailPos=0;};
	 bool IsOverflow(DWORD length);			// Estimation of Buffersize overflow
	 DWORD GetHeadPos() { return m_dwHeadPos;};
	 DWORD GetTailPos() { return m_dwTailPos;};

private:
	char* m_pBuffer;
	DWORD m_dwBufferSize;
	DWORD m_dwHeadPos;
	DWORD m_dwTailPos;
};

