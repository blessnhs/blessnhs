#include "StdAfx.h"
#include "GSEventSelect.h"

namespace GSNetwork { namespace GSEventSelect	{

DWORD WINAPI SelectThreadCallback(LPVOID parameter)
{
	GSEventSelect *Owner = (GSEventSelect*) parameter;
	Owner->SelectThreadCallback();

	return 0;
}

VOID GSEventSelect::SelectThreadCallback(VOID)
{
	WSANETWORKEVENTS	NetworkEvents;
	DWORD				EventID = 0;
	DWORD				Result	= 0;
	HANDLE				ThreadEvents[2] = {m_DestroyEventHandle, m_SelectEventHandle};

	while (TRUE)
	{
		SetEvent(m_StartupEventHandle);

		EventID = ::WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
		switch (EventID)
		{
		case WAIT_OBJECT_0:
			return;

		case WAIT_OBJECT_0 + 1:
			EventID = WSAEnumNetworkEvents(m_Socket, m_SelectEventHandle, &NetworkEvents);

			if (EventID == SOCKET_ERROR)
				return;
			else
			{
				if (NetworkEvents.lNetworkEvents & FD_CONNECT)
				{
					OnIoConnected();

					if (NetworkEvents.iErrorCode[FD_CONNECT_BIT])
						return;
				}
				else if (NetworkEvents.lNetworkEvents & FD_WRITE)
				{
					//OnIoWrote();
				}
				else if (NetworkEvents.lNetworkEvents & FD_READ)
				{
					OnIoRead();
				}
				else if (NetworkEvents.lNetworkEvents & FD_CLOSE)
				{
					OnIoDisconnected();

					return;
				}
			}
			break;
		default:
			return;
		}
	}
}

GSEventSelect::GSEventSelect(VOID)
{
	m_SelectEventHandle	= NULL;
	m_DestroyEventHandle	= NULL;
	m_SelectThreadHandle	= NULL;
	m_StartupEventHandle	= NULL;

	m_Socket				= NULL;
}

GSEventSelect::~GSEventSelect(VOID)
{
}

BOOL GSEventSelect::Initialize(SOCKET socket)
{
	if (!socket)
		return FALSE;

	if (m_Socket)
		return FALSE;

	m_Socket = socket;

	m_SelectEventHandle = WSACreateEvent();
	if (m_SelectEventHandle == WSA_INVALID_EVENT)
	{
		Termination();

		return FALSE;
	}

	m_DestroyEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (m_DestroyEventHandle == NULL)
	{
		Termination();

		return FALSE;
	}

	m_StartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (m_StartupEventHandle == NULL)
	{
		Termination();

		return FALSE;
	}

	DWORD Result = WSAEventSelect(socket, m_SelectEventHandle, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (Result == SOCKET_ERROR)
	{
		Termination();

		return FALSE;
	}

	DWORD SelectThreadID	= 0;
	m_SelectThreadHandle		= CreateThread(NULL, 0, GSNetwork::GSEventSelect::SelectThreadCallback, this, 0, &SelectThreadID);

	if (!m_SelectThreadHandle)
	{
		Termination();

		return FALSE;
	}

	// Thread가 Wait될때까지 시간을 벌어준다.
	WaitForSingleObject(m_StartupEventHandle, INFINITE);

	return TRUE;
}

BOOL GSEventSelect::Termination(VOID)
{
	if (!m_Socket)
		return FALSE;

	if (m_SelectThreadHandle)
	{
		SetEvent(m_DestroyEventHandle);

		WaitForSingleObject(m_SelectThreadHandle, INFINITE);

		CloseHandle(m_SelectThreadHandle);
	}

	if (m_SelectEventHandle)
		CloseHandle(m_SelectEventHandle);

	if (m_DestroyEventHandle)
		CloseHandle(m_DestroyEventHandle);

	if (m_StartupEventHandle)
		CloseHandle(m_StartupEventHandle);

	return TRUE;
}

}	}	