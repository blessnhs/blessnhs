#pragma once

#include <map>
#include "../iocp/TCP/ServerIOCP_TCP.h"

namespace GSRelayServer	{	namespace Group	{

struct GroupStock
{
	WORD Id;
	int Type;
	WORD MaxUser;
	WORD CurrentUser;

	BOOL Use;
};

struct Member
{
	Member()
	{
		memset(this,0,sizeof(Member));
	}

	DWORD Id;			//uid
	DWORD Key;			//ukey
	DWORD TCP_ID;		
	DWORD Port;

	bool  assign;
	SOCKADDR_IN		m_UdpRemoteInfo;
};

struct ConnectionPairMember
{
	Member _Member[2];

	bool IsOK()
	{
		if(_Member[0].assign ==  false)
			return false;

		if(_Member[1].assign ==  false)
			return false;

		return TRUE;
	}
};


class Group 
{
public:
	Group(void);
	~Group(void);

	GroupStock			m_Stock;

	VOID BroadCastUDP(char *Message);

	VOID BroadCastTCP(WORD MID,CONST char *Msg,WORD Size)
	{
		std::map<DWORD,Member>::iterator ITER = 	m_MemberList.begin();

		while(ITER != m_MemberList.end())
		{
			TCP_SESSION *pSession = IOCP_TCP.GetSessionMgr()->GetSession(ITER->second.TCP_ID);

			if(pSession != NULL)
			{
				//Sender::Send(pSession,Message);
				pSession->WritePacket(MID,0,(BYTE *)Msg,Size);
			}

			ITER++;
		}
	}

	ConnectionPairMember* GetPairMember(DWORD Id)
	{
		for each( auto pair in m_ConnectionPairList)
		{
		//	if(pair._Member[0].Id != pair._Member[1].Id)
		//		return pair;

			if(pair->_Member[0].Id  == Id)
				return pair;
		}

		return NULL;
	}

	VOID EraseByTCPId(DWORD TCPId)
	{
		for each( auto member in m_MemberList)
		{
			if(member.second.TCP_ID == TCPId)
			{
				m_MemberList.erase(member.second.Id);
				break;
			}
		}
	}

	std::vector<ConnectionPairMember*>		m_ConnectionPairList;
	std::map<DWORD,Member>					m_MemberList;
};

#define GROUP GSRelayServer::Group::Group 
#define GROUP_PTR  boost::shared_ptr<GROUP>


}	}