#include "StdAfx.h"

#include "GSServerAgent.h"

#include "GSSerialize.h"
#include "../LOG/FrontLog.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "../GSPacket/Front.h"

#include "../Server/GSJGServerProcess.h"

GSServerAgent::GSServerAgent(void)
{
	Create(TCP);

	SetType(_SERVER_);

	boost::shared_ptr<GSJGServerProcess> process(new GSJGServerProcess);
	SetProcess(process);

}


GSServerAgent::~GSServerAgent(void)
{
}

BOOL GSServerAgent::Recycle(SOCKET hListenSocket)
{
	CThreadSync Sync;


	
	GetTCPSocket()->Termination();

	return GetTCPSocket()->GSPacket::Initialize() && GetTCPSocket()->Accept(hListenSocket);

}


VOID GSServerAgent::Connect()
{
	if(GetConnected() == TRUE) 
		return ;

	m_GSServer = &JGSERVER;

	CHAR Ip[MAX_IP_ADDRESS];

	WideCharToMultiByte(CP_ACP, 0, INI.m_UnityServerIp, -1, Ip, MAX_IP_ADDRESS, NULL, NULL); 

	GetTCPSocket()->CreateTcpSocket();
	
	BOOL Ret = GetTCPSocket()->Connect(Ip,INI.m_UnityPort);
	
	JGSERVER.RegIocpHandler(GetSocket(),(ULONG_PTR)this);

	GetTCPSocket()->InitializeReadForIocp();

	if(Ret == TRUE)
	{
		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Type",FRONT_SERVER )

		int Id = INI.m_Id;

		ADD_JSON_MEMBER("Id",Id)

		END_JSON_MEMBER(this->GetTCPSocket(),ID_GF_PKT_REGIST_REQ)

/*		GF_PKT_REGIST_REQ snd;
		snd.Type  = Adapt::E_TYPE::FRONT_SERVER;
		snd.Id  = GetServerIOCP().m_IniInfo.m_Id;

		this->WritePacket(ID_GF_PKT_REGIST_REQ,0,(BYTE*)&snd,sizeof(snd));
*/
		SetConnected(TRUE);
	}
	else
		
	{
		SetConnected(FALSE);
		printf("GS Server Connect Failed %s %d\n",Ip,INI.m_UnityPort);
	}
}