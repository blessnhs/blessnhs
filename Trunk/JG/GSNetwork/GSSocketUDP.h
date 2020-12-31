#pragma once


namespace GSNetwork	{ namespace GSSocket	{	namespace GSSocketUDP	{

class GSSocketUDP :	public GSSocket
{
public:
	GSSocketUDP(void);
	~GSSocketUDP(void);

	BOOL Initialize(VOID);
	BOOL Termination(VOID);

	BOOL CreateUdpSocket(USHORT port);

	BOOL InitializeReadFromForIocp(VOID);

	BOOL ReadFromForIocp(LPSTR& remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength);
	BOOL ReadFromForEventSelect(LPSTR remoteAddress, USHORT &remotePort, BYTE *data, DWORD &dataLength);
	BOOL WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength);
	BOOL WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE *data, DWORD dataLength);
	BOOL GetRemoteAddressAfterAccept(LPTSTR remoteAddress, USHORT &remotePort);
	
	VOID ReliableUdpThreadCallback(VOID);

	SOCKADDR_IN		m_UdpRemoteInfo;

private:
	
	HANDLE			m_ReliableUdpThreadHandle;
	HANDLE			m_ReliableUdpThreadStartupEvent;
	HANDLE			m_ReliableUdpThreadDestroyEvent;
	HANDLE			m_ReliableUdpThreadWakeUpEvent;
	HANDLE			m_ReliableUdpWriteCompleteEvent;

	std::queue<std::shared_ptr<WRITE_PACKET_INFO>>		m_ReliableWriteQueue;					
	BOOL			m_IsReliableUdpSending;
	
	inline VOID		SetUdpWriteCompleteEvent(VOID);
};

}	}	}
