#include "StdAfx.h"
#include "IUdpProcess.h"
#include "../GSPacket/Relay.h"
#include "GSSerialize.h"
#include "Sender.h"
#include "../group/GroupContainer.h"
#include "../Allocator/GSAllocator.h"
#include "../GROUP/Group.h"
#include "../IOCP/TCP/ServerIOCP_TCP.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSSerialize.h"

IUdpProcess::IUdpProcess(void)
{
	ADD_NET_FUNC(IUdpProcess,ID_GR_PKT_REGIST_RES,ACCESS_RELAYSVR);
	ADD_NET_FUNC(IUdpProcess,ID_CR_BROADCAST_NTY,BROADCAST_NTY);
	ADD_NET_FUNC(IUdpProcess,ID_RC_CREATE_UDP_RES,CREATE_UDP_SOCKET_NTY);
}

IUdpProcess::~IUdpProcess(void)
{
}


VOID IUdpProcess::Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol)
{
	HEADER *pHeader = (HEADER *)Data;

	NET_FUNC_EXE(MainProtocol,Data,Length);
}

VOID IUdpProcess::ACCESS_RELAYSVR(LPVOID Data,DWORD Length)
{
	DESERIALIZE_BEGIN(Data,FR_PKT_REGIST_RES,pRes,Length)

	printf("Regist Success GSServer \n");

	DESERIALIZE_END
}

VOID IUdpProcess::CLOSE_RELAYSVR(LPVOID Data,DWORD Length)
{
}

VOID IUdpProcess::CREATE_UDP_SOCKET_NTY(LPVOID Data,DWORD Length)
{
	DECLARE_JSON_READ
	GET_JSON_UINT_MEMBER("Id",Id)
	GET_JSON_UINT_MEMBER("GroupIdx",GroupId)

	UDP_SESSION *CurrentSession = (UDP_SESSION *)pOwner;

	char *ch =  inet_ntoa(CurrentSession->m_UdpRemoteInfo.sin_addr); 
	printf("ip = %s port = %d user id = %d CREATE_UDP_SOCKET_NTY\n",ch,CurrentSession->m_UdpRemoteInfo.sin_port,Id);

	//해당 유저가 진짜 유저인지 확인 여부
	//그룹 입장 판단 여부 tcp소켓의 그 유저인지 유효성 판단이 필요함.

	GROUP_PTR pGroup = GROUPCONTAINER.Search(GroupId);
	if(pGroup != NULL)
	{

		for each( auto member_ptr in pGroup->m_MemberList)
		{
			if(member_ptr.first == Id)
			{
				pGroup->m_MemberList[Id].m_UdpRemoteInfo = CurrentSession->m_UdpRemoteInfo;
				pGroup->m_MemberList[Id].assign = true;
			}
		}

		for each( auto ConnectionPairMember in pGroup->m_ConnectionPairList)
		{
			if(ConnectionPairMember->_Member[0].Id == Id)
			{
				ConnectionPairMember->_Member[0].m_UdpRemoteInfo = CurrentSession->m_UdpRemoteInfo;
				ConnectionPairMember->_Member[0].assign = true;
			}

			if(ConnectionPairMember->_Member[1].Id == Id)
			{
				ConnectionPairMember->_Member[1].m_UdpRemoteInfo = CurrentSession->m_UdpRemoteInfo;
				ConnectionPairMember->_Member[1].assign = true;
			}
		}

	/*	if(pGroup->m_MemberList.find(Id) == pGroup->m_MemberList.end())
			return ;

		pGroup->m_MemberList[Id].m_UdpRemoteInfo = CurrentSession->m_UdpRemoteInfo;
		pGroup->m_MemberList[Id].assign = true;*/
	}

	if(pGroup != NULL)
	{
		for each( auto member in pGroup->m_ConnectionPairList)
		{
			if(member->_Member[0].Id == Id || member->_Member[1].Id == Id)
			{
				if( member->IsOK() == TRUE)	//2명 이상이다.
				{
					RC_PKT_UDP_REQUEST_STUN SND;

					//src user에서 홀펀칭 해야하는 유저를 보낸다.
					{
						char *ds =  inet_ntoa(member->_Member[1].m_UdpRemoteInfo.sin_addr);	

						WCHAR DestIP[20];
						MultiByteToWideChar(CP_ACP,0, ds, 20, DestIP, 20);

						TCP_SESSION *pSrcSession = IOCP_TCP.GetSessionMgr()->GetSession(member->_Member[0].TCP_ID);
						DECLARE_JSON_WRITER
						wstring _Destip = DestIP;
						ADD_JSON_WSTR_MEMBER("Ip",_Destip)
						ADD_JSON_MEMBER("Port",member->_Member[1].m_UdpRemoteInfo.sin_port)
						ADD_JSON_MEMBER("GroupID",GroupId)
						ADD_JSON_MEMBER("Index",(int)member->_Member[1].Id);
						END_JSON_MEMBER(pSrcSession,ID_RC_REQUEST_STUN)

						printf("===============================================\n");
						printf("send src %S_%d Index %d\n",_Destip.c_str(),member->_Member[1].m_UdpRemoteInfo.sin_port,member->_Member[0].Id);
					}
					//dest 이다.
					{
						char *ds =  inet_ntoa(member->_Member[0].m_UdpRemoteInfo.sin_addr);	

						WCHAR DestIP[20];
						MultiByteToWideChar(CP_ACP,0, ds, 20, DestIP, 20);
						wstring _Destip = DestIP;
						TCP_SESSION *pSession2 = IOCP_TCP.GetSessionMgr()->GetSession(member->_Member[1].TCP_ID);
						DECLARE_JSON_WRITER
						ADD_JSON_WSTR_MEMBER("Ip",_Destip)
						ADD_JSON_MEMBER("Port",member->_Member[0].m_UdpRemoteInfo.sin_port)
						ADD_JSON_MEMBER("GroupID",GroupId)
						ADD_JSON_MEMBER("Index",(int)member->_Member[0].Id);
						END_JSON_MEMBER(pSession2,ID_RC_REQUEST_STUN)

						printf("send src %S_%d Index %d\n",_Destip.c_str(),member->_Member[0].m_UdpRemoteInfo.sin_port,member->_Member[1].Id);
						printf("===============================================\n");

					}
					
				}
			}
		}
	}

}

VOID IUdpProcess::BROADCAST_NTY(LPVOID Data,DWORD Length)
{
	CR_PKT_BROADCAST_NTY *pRes = (CR_PKT_BROADCAST_NTY *)Data;

	GROUP_PTR pGroupPtr = GROUPCONTAINER.Search(pRes->Id) ;

	if(pGroupPtr!= NULL)
	{
		std::map<DWORD,GSRelayServer::Group::Member>::iterator iter = pGroupPtr->m_MemberList.begin();
	/*	while(iter != pGroupPtr->m_MemberList.end())
		{
			pOwner->WriteTo2(inet_ntoa(iter->m_UdpRemoteInfo.sin_addr),iter->m_UdpRemoteInfo.sin_port,(BYTE *)pRes,sizeof(iter->m_UdpRemoteInfo.sin_port,pRes));

			iter++;
		}*/
	}
}