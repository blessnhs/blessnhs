#pragma once

#include "SingleTonHolder.h"

struct SeverStock
{
	DWORD Port;
	DWORD Id;
	char IpAddress[MAX_IP_ADDRESS];
	CHAR ChannelName[MAX_CHANNEL_NAME_LEGNTH];
	CHAR ServerName[MAX_CHANNEL_NAME_LEGNTH];
	int Type;
	WORD MaxUser;
	WORD CurrentUser;
	BOOL Use;

};

class GSServerAgent : public GSCLIENT , public Singleton<GSServerAgent>
{
public:
	GSServerAgent(void);
	~GSServerAgent(void);

	BOOL Recycle(SOCKET hListenSocket);

	VOID Connect();

	SeverStock m_Stock;
};

#define  UNITY Singleton<GSServerAgent>::Instance()
