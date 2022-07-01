#pragma once


#include "GSClientModule.h"
#include "GSBuffer.h"

class GSClientSocket : public GSClientModule
{
public:
	GSClientSocket(void);
	~GSClientSocket(void);

	virtual VOID	OnIoRead(VOID);
	virtual VOID	OnIoConnected(VOID);
	virtual VOID	OnIoDisconnected(VOID);

	

	byte			Position;


	std::wstring id;
	std::wstring sessionkey;
};

extern GSClientSocket m_SocketLogin;
extern GSClientSocket m_SocketLobby;
extern GSClientSocket m_SocketRelay;
