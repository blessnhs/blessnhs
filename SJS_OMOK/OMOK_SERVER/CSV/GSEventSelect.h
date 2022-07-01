#pragma once

class GSEventSelect
{
public:
	GSEventSelect(void);
	virtual ~GSEventSelect(void);

private:
	HANDLE			m_SelectEventHandle;
	HANDLE			m_StartupEventHandle;
	HANDLE			m_DestroyEventHandle;

	HANDLE			m_SelectThreadHandle;

	SOCKET			m_Socket;

protected:
	virtual VOID	OnIoRead(VOID)			= 0;
	//virtual VOID	OnIoWrote(VOID)			= 0;
	virtual VOID	OnIoConnected(VOID)		= 0;
	virtual VOID	OnIoDisconnected(VOID)	= 0;

public:
	BOOL			Initialize(SOCKET socket);
	BOOL			Termination(VOID);

	VOID			SelectThreadCallback(VOID);
};
