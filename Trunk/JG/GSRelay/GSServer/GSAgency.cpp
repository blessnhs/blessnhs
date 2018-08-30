#include "StdAfx.h"
#include "GSAgency.h"
#include "../IOCP/TCP/ServerIOCP_TCP.h"
#include "../GSPacket/Relay.h"
#include "../GSIniContext.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

#include "GSSerialize.h"


namespace GSRelayServer	{	namespace GSAgency	{

GSAgency::GSAgency(void)
{
	SetType(_GS_SERVER_);
}

GSAgency::~GSAgency(void)
{
}

BOOL GSAgency::Recycle(SOCKET hListenSocket)
{
	CThreadSync Sync;

	Termination();

	return GSPacket::Initialize() && Accept(hListenSocket);

}

void GSAgency::OnConnect()
{
	printf("socket connected\n");

	
}


VOID GSAgency::ConnectGS()
{
	if(GetConnected() == TRUE) return ;

	Termination();
	CreateTcpSocket();

	CHAR IP[128];
	wsprintfA( IP, "%S", INICONTEXT.GSIP );
	
	BOOL Ret = Connect(IP,INICONTEXT.GSPort);

	IOCP_TCP.RegIocpHandler(GetSocket(),(ULONG_PTR)this);

	InitializeReadForIocp();

	if(Ret == TRUE)
	{
	/*	RF_PKT_REGIST_REQ snd;
		snd.Type  = RELAY_SERVER;
		snd.Id  = INICONTEXT.m_Id;

		Send(this,snd);
	*/
		DECLARE_JSON_WRITER
		ADD_JSON_MEMBER("Type",RELAY_SERVER )
		int Id = INICONTEXT.m_Id;
		ADD_JSON_MEMBER("Id",Id)
		END_JSON_MEMBER(this,ID_GF_PKT_REGIST_REQ)

		SetConnected(TRUE);
	}
	else
		
	{
		SetConnected(FALSE);
		printf("GS Server Connect Failed %s %d\n",m_Stock.IpAddress,m_Stock.Port);
	}
}
}	}