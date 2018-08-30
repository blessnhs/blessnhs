#pragma once

#include "../IOCP/TCP/TCPSession.h"
#include "SingleTonHolder.h"

namespace GSRelayServer	{	namespace GSAgency	{

struct GSStock
{
	char IpAddress[MAX_IP_ADDRESS];
	DWORD Port;
	DWORD Id;
	CHAR ChannelName[MAX_CHANNEL_NAME_LEGNTH];
	int Type;

	WORD MaxUser;

	CHAR ServerName[MAX_CHANNEL_NAME_LEGNTH];

	WORD CurrentUser;

	BOOL Use;

};

class GSAgency : public TCP_SESSION
{
public:

	enum E_TYPE
	{
		DBA	=	0,
		FIELD_SERVER,
		FRONT_SERVER,
		RELAY_SERVER,
	};

	GSAgency(void);
	~GSAgency(void);

	BOOL Recycle(SOCKET hListenSocket);

	VOID ConnectGS();

	virtual void OnConnect();

	GSStock m_Stock;


	template<class T>
	static void Send(TCP_SESSION *pSession,T &C)
	{
		if(pSession == NULL) return ;

		std::stringstream m_Stream; 
		boost::archive::text_oarchive oa(m_Stream);

		oa << C;

		HEADER hdr = (HEADER)C;

		pSession->WritePacket(hdr.MainId,hdr.SubId,(BYTE*)m_Stream.str().c_str(),(WORD)m_Stream.str().size());
	}

protected:

	DECLARE_MSG_MAP(GSAgency)
};

#define GSS Singleton<GSRelayServer::GSAgency::GSAgency>::Instance()




}	}