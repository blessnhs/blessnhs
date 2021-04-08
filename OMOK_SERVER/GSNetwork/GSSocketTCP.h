#pragma once

namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketTCP	{

class GSSocketTCP : public GSSocket
{
public:
	GSSocketTCP(void);
	virtual ~GSSocketTCP(void);

	BOOL Initialize(VOID);
	BOOL Termination(VOID);
	
	int  ReadForIocp(BYTE * PacketBuffer, DWORD DataLength,DWORD RemainLength, DWORD MaxPacketBufferSize);
	BOOL ReadForEventSelect(BYTE *Data, DWORD &DataLength);
	BOOL Write(BYTE *Data, DWORD DataLength);


	BOOL CreateTcpSocket(VOID);
	BOOL InitializeReadForIocp(VOID);
	BOOL GetPeerInfo(LPSTR szAddr, unsigned short &usPort);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);
};

}	}	}