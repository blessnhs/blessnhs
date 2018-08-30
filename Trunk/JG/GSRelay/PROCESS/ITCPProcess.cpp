#include "StdAfx.h"
#include "ITCPProcess.h"
#include "../GSPacket/Relay.h"
#include "GSSerialize.h"
#include "Sender.h"
#include "../group/GroupContainer.h"
#include "../iocp/UDP/ServerIOCP_UDP.h"
#include "../iocp/TCP/ServerIOCP_TCP.h"
#include "../GSIniContext.h"
#include "../Allocator/GSAllocator.h"
#include "Sender.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSSerialize.h"

ITCPProcess::ITCPProcess(void)
{

	ADD_NET_FUNC(ITCPProcess,ID_CR_LOGIN_REQ,LOGIN_REQ);
	ADD_NET_FUNC(ITCPProcess,ID_CR_ENTER_ROOM_REQ,ENTER_ROOM_REQ);
	ADD_NET_FUNC(ITCPProcess,ID_CR_LEAVE_ROOM_REQ,LEAVE_ROOM_REQ);
}

ITCPProcess::~ITCPProcess(void)
{
}

VOID ITCPProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	NET_FUNC_EXE(MainProtocol,Data,Length);
}

VOID ITCPProcess::LOGIN_REQ(LPVOID Data,DWORD Length)
{
	DESERIALIZE_BEGIN(Data,CR_PKT_LOGIN_REQ,pRes,Length)

	DESERIALIZE_END
}


VOID ITCPProcess::ENTER_ROOM_REQ(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	
	GET_JSON_UINT_MEMBER("Id",GroupId)
	GET_JSON_UINT_MEMBER("PlayerIndex",PlayerIndex)

	GROUP_PTR pGroupPtr = GROUPCONTAINER.Search(GroupId) ;

	if(pGroupPtr!= NULL)
	{
		TCP_SESSION *TCPSession = (TCP_SESSION *)pOwner;

		GSRelayServer::Group::Member member;
		member.Key = GroupId;
		member.Id  = PlayerIndex;//TCPSession->GetId();
		member.TCP_ID = TCPSession->GetId();;
		TCPSession->m_AssociateSocket.m_GroupNumber = pGroupPtr->m_Stock.Id;

		//기존 멤버들에게 새로운 유저와 페어를 맺어준다.
		for each( auto member_ptr in pGroupPtr->m_MemberList)
		{
			GSRelayServer::Group::ConnectionPairMember *pair = new GSRelayServer::Group::ConnectionPairMember;
			pair->_Member[0] = member_ptr.second;
			pair->_Member[1] = member;

			pGroupPtr->m_ConnectionPairList.push_back(pair);

			printf("Add ConnectionPair [0] = %d [1] = %d \n",pair->_Member[0].Id,pair->_Member[1].Id);
		}
		//새로운 유저에게 기존 멤버와 페어를 맺어 준다.
		for each( auto member_ptr in pGroupPtr->m_MemberList)
		{
			GSRelayServer::Group::ConnectionPairMember *pair = new GSRelayServer::Group::ConnectionPairMember;
			pair->_Member[0] = member;
			pair->_Member[1] = member_ptr.second;

			pGroupPtr->m_ConnectionPairList.push_back(pair);

			printf("Add ConnectionPair [0] = %d [1] = %d \n",pair->_Member[0].Id,pair->_Member[1].Id);
		}

		//기존 멤버들 정보를 새로운 유저에게 전송
		for each( auto member in pGroupPtr->m_MemberList)
		{
			DECLARE_JSON_WRITER
			ADD_JSON_MEMBER("Id",(WORD)member.second.Id)
			ADD_JSON_MEMBER("Port",(WORD)member.second.Port)
			ADD_JSON_MEMBER("Result",0)
			ADD_JSON_MEMBER("GroupID",(WORD)pGroupPtr->m_Stock.Id)
			END_JSON_MEMBER(TCPSession,ID_RC_ENTER_ROOM_RES)
		}

		pGroupPtr->m_MemberList[member.Id] = (member);

		WORD Port = IOCP_UDP.m_FreePorts.back();
		IOCP_UDP.m_FreePorts.pop_back();

		member.Port = Port;

		TCPSession->m_AssociateSocket.m_AllocPort = Port;

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Id",(WORD)member.Id)
		ADD_JSON_MEMBER("Port",Port)
		ADD_JSON_MEMBER("Result",0)
		ADD_JSON_MEMBER("GroupID",(WORD)pGroupPtr->m_Stock.Id)
	//	END_JSON_MEMBER(TCPSession,ID_RC_ENTER_ROOM_RES)
		Json::FastWriter writer;
		std::string outputConfig = writer.write( root2 );	
		pGroupPtr->BroadCastTCP(ID_RC_ENTER_ROOM_RES,outputConfig.c_str(),outputConfig.size());

		printf("=============Enter Group Id = %d Player Id = %d \n", pGroupPtr->m_Stock.Id,member.Id);
	}
	else
	{
		GROUP_PTR	pGroup = ALLOCATOR.Create<GROUP>(Type2Type<GROUP>());
		GROUPCONTAINER.Add(pGroup);

		TCP_SESSION *TCPSession = (TCP_SESSION *)pOwner;

		GSRelayServer::Group::Member member;
		member.Key = pGroup->m_Stock.Id;
		member.Id  = PlayerIndex;//TCPSession->GetId();
		member.TCP_ID = TCPSession->GetId();;
		pGroup->m_MemberList[member.Id] = (member);

		TCPSession->m_AssociateSocket.m_GroupNumber = pGroup->m_Stock.Id;

		pGroup->m_MemberList[member.Id] = (member);
		
		WORD Port = IOCP_UDP.m_FreePorts.back();
		IOCP_UDP.m_FreePorts.pop_back();

		TCPSession->m_AssociateSocket.m_AllocPort = Port;

		printf("=============Create Group Id = %d Player Id = %d \n", pGroup->m_Stock.Id,member.Id);

		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Id",(WORD)member.Id)
		ADD_JSON_MEMBER("Port",Port)
		ADD_JSON_MEMBER("Result",0)
		ADD_JSON_MEMBER("GroupID",(WORD)pGroup->m_Stock.Id)
		END_JSON_MEMBER(TCPSession,ID_RC_ENTER_ROOM_RES)
	}
}


VOID ITCPProcess::LEAVE_ROOM_REQ(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_UINT_MEMBER("Id",Id)

	GROUP_PTR pGroupPtr = GROUPCONTAINER.Search(Id) ;

	if(pGroupPtr!= NULL)
	{
		std::map<DWORD,GSRelayServer::Group::Member>::iterator iter = pGroupPtr->m_MemberList.begin();
		while(iter != pGroupPtr->m_MemberList.end())
		{
			if(iter->first == pOwner->GetId())
			{
				iter = pGroupPtr->m_MemberList.erase(iter);
			}
			else
				++iter;
		}
	}

}

VOID ITCPProcess::BROADCAST_NTY(LPVOID Data,DWORD Length)
{

}

VOID ITCPProcess::SUCCESS_STUN(LPVOID Data,DWORD Length)
{

}

VOID ITCPProcess::REPORT_STUN(LPVOID Data,DWORD Length)
{
	//성공 실패에 따라 relay udp relay tcp를 결정한다.
	
}