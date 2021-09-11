#include "StdAfx.h"
#include "IUdpProcess.h"


enum UDP_PROTOCOL
{
	ENTER_ROOM = 0,
};

IUdpProcess::IUdpProcess(void)
{
//	ADD_NET_FUNC(IUdpProcess, UDP_PROTOCOL::ENTER_ROOM,ENTER_ROOM);
//	ADD_NET_FUNC(IUdpProcess,ID_CR_BROADCAST_NTY,BROADCAST_NTY);
//	ADD_NET_FUNC(IUdpProcess,ID_RC_CREATE_UDP_RES,CREATE_UDP_SOCKET_NTY);
}

IUdpProcess::~IUdpProcess(void)
{
}


VOID IUdpProcess::Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport)
{
	
}
//
VOID IUdpProcess::ENTER_ROOM(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pClient)
{
}


//
//VOID IUdpProcess::CLOSE_RELAYSVR(LPVOID Data,DWORD Length)
//{
//}
//
//VOID IUdpProcess::CREATE_UDP_SOCKET_NTY(LPVOID Data,DWORD Length)
//{
//	DECLARE_JSON_READ
//	GET_JSON_UINT_MEMBER("Id",Id)
//	GET_JSON_UINT_MEMBER("GroupIdx",GroupId)
//
//	UDP_SESSION *CurrentSession = (UDP_SESSION *)pOwner;
//
//	char *ch =  inet_ntoa(CurrentSession->m_UdpRemoteInfo.sin_addr); 
//	SYSLOG().Write("ip = %s port = %d user id = %d CREATE_UDP_SOCKET_NTY\n",ch,CurrentSession->m_UdpRemoteInfo.sin_port,Id);
//
//	해당 유저가 진짜 유저인지 확인 여부
//	그룹 입장 판단 여부 tcp소켓의 그 유저인지 유효성 판단이 필요함.
//
//	GROUP_PTR pGroup = GROUPCONTAINER.Search(GroupId);
//	if(pGroup != NULL)
//	{
//		if(pGroup->m_MemberList.find(Id) == pGroup->m_MemberList.end())
//			return ;
//
//		pGroup->m_MemberList[Id].m_UdpRemoteInfo = CurrentSession->m_UdpRemoteInfo;
//		pGroup->m_MemberList[Id].assign = true;
//	}
//
//	GROUP_PTR pGroup = GROUPCONTAINER.Search(pRes->GroupId);
//	if(pGroup != NULL)
//	{
//		if( pGroup->m_MemberList.size() > 1)	//2명 이상이다.
//		{
//			RC_PKT_UDP_REQUEST_STUN SND;
//			wsprintf(SND.SrcIP,"%S", ch);
//
//			MultiByteToWideChar(CP_ACP,0, ch, 20, SND.SrcIP, 20);	//요청한 session 주소를 wide char 변환 한다.
//
//			SND.SrcPort = CurrentSession->m_UdpRemoteInfo.sin_port;
//
//			std::map<DWORD,GSRelayServer::Group::Member>::iterator ITER = 	pGroup->m_MemberList.begin();
//			while(ITER != pGroup->m_MemberList.end())
//			{
//				if(ITER->first != Id)
//				{
//					if(ITER->second.assign != false)
//					{
//				
//						SND.DestPort = ITER->second.m_UdpRemoteInfo.sin_port;				//요청한 유저와 다른 유저의 포트와 ip정보
//						char *ds =  inet_ntoa(ITER->second.m_UdpRemoteInfo.sin_addr);		//ITER가 DEST이다.
//						memcpy(SND.DestIP , ds,20);										//Id SRC이다.
//
//						MultiByteToWideChar(CP_ACP,0, ds, 20, SND.DestIP, 20);
//
//						src user에서 홀펀칭 해야하는 유저를 보낸다.
//						{
//							TCP_SESSION *pSrcSession = IOCP_TCP.GetSessionMgr()->GetSession(Id);		
//							DECLARE_JSON_WRITER
//							wstring _Destip = SND.DestIP;
//							ADD_JSON_WSTR_MEMBER("Ip",_Destip)
//							ADD_JSON_MEMBER("Port",SND.DestPort)
//							ADD_JSON_MEMBER("GroupID",GroupId)
//							END_JSON_MEMBER(pSrcSession,ID_RC_REQUEST_STUN)
//
//							SYSLOG().Write("===============================================\n");
//							SYSLOG().Write("send src %S_%d\n",_Destip.c_str(),SND.DestPort);
//						}
//						dest 이다.
//						{
//							wstring _SRCip = SND.SrcIP;
//							TCP_SESSION *pSession2 = IOCP_TCP.GetSessionMgr()->GetSession(ITER->first);
//							DECLARE_JSON_WRITER
//							ADD_JSON_WSTR_MEMBER("Ip",_SRCip)
//							ADD_JSON_MEMBER("Port",SND.SrcPort)
//							ADD_JSON_MEMBER("GroupID",GroupId)
//							END_JSON_MEMBER(pSession2,ID_RC_REQUEST_STUN)
//
//							SYSLOG().Write("send src %S_%d\n",_SRCip.c_str(),SND.SrcPort);
//							SYSLOG().Write("===============================================\n");
//
//						}
//					}
//				
//				}
//				ITER++;
//			}
//		}
//	}
//
//}
//
//VOID IUdpProcess::BROADCAST_NTY(LPVOID Data,DWORD Length)
//{
//	CR_PKT_BROADCAST_NTY *pRes = (CR_PKT_BROADCAST_NTY *)Data;
//
//	GROUP_PTR pGroupPtr = GROUPCONTAINER.Search(pRes->Id) ;
//
//	if(pGroupPtr!= NULL)
//	{
//		std::map<DWORD,GSRelayServer::Group::Member>::iterator iter = pGroupPtr->m_MemberList.begin();
//	/*	while(iter != pGroupPtr->m_MemberList.end())
//		{
//			pOwner->WriteTo2(inet_ntoa(iter->m_UdpRemoteInfo.sin_addr),iter->m_UdpRemoteInfo.sin_port,(BYTE *)pRes,sizeof(iter->m_UdpRemoteInfo.sin_port,pRes));
//
//			iter++;
//		}*/
//	}
//}