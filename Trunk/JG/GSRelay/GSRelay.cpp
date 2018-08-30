// GSRelay.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "iocp/UDP/ServerIOCP_UDP.h"
#include "iocp/TCP/ServerIOCP_TCP.h"
#include "GSIniContext.h"
#include "GSServer/GSAgency.h"
#include "GSMainProcess.h"

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);	

	INICONTEXT.Load(_T("./Relay.ini"));

	MAINPROC->Create(1);
	MAINPROC->Activate(GSRelayServer::GSMainProcess::GSMainProcess::MSG_TYPE_USER ,1);
	MAINPROC->Activate(GSRelayServer::GSMainProcess::GSMainProcess::MSG_TYPE_UDP_1 ,1);
	MAINPROC->Activate(GSRelayServer::GSMainProcess::GSMainProcess::MSG_TYPE_LOG  ,1);

	printf("GSRealy Server Port = %d MaxPlayer = %d UDP_COUNT = %d \nUDP_PORT = %d\n GSServer Ip = %S GSServer Port = %d\n",
		INICONTEXT.m_Port,INICONTEXT.m_MaxPlayerCount,INICONTEXT.m_UdpCount,INICONTEXT.m_UdpPort,INICONTEXT.GSIP,INICONTEXT.GSPort);

	IOCP_UDP.Begin();
	IOCP_TCP.Begin(); 

	GSS.ConnectGS();

	char ch;
	scanf(&ch,"%c");

	return 0;
}

