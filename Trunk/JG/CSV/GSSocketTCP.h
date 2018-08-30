#pragma once

#include "GSSocket.h"

namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketTCP	{

class GSSocketTCP : public GSSocket
{
public:
	GSSocketTCP(void);
	~GSSocketTCP(void);

	BOOL Initialize(VOID);
	BOOL Termination(VOID);
	
	BOOL ReadForIocp(BYTE *data, DWORD &dataLength,DWORD RemainLength);
	BOOL ReadForEventSelect(BYTE *data, DWORD &dataLength);
	BOOL Write(BYTE *data, DWORD dataLength);


	BOOL CreateTcpSocket(VOID);
	BOOL InitializeReadForIocp(VOID);
	BOOL GetPeerInfo(LPSTR szAddr, unsigned short &usPort);

	VOID SetConnected(BOOL bConnected);
	BOOL GetConnected(VOID);

protected:

	BOOL m_bConnected;
};

}	}	}