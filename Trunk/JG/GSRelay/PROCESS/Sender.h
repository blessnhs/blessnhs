#pragma once

#include "../IOCP/TCP/TCPSession.h"

class Sender
{
public:
	Sender() {}
	~Sender() {}

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
};