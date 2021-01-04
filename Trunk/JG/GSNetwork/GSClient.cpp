#include "StdAfx.h"
#include "GSClient.h"
#include "GSMainProcess.h"
#include "MSG_PLAYER_CLOSE.h"
#include "MSG_PLAYER_ACCEPT.h"
#include "MSG_PLAYER_PACKET.h"

#include "GSAllocator.h"
#include "GSServer.h"
#include "IUdpProcess.h"

//namespace GSNetwork	{	namespace GSClient	{

atomic<int>		DebugCount = 0;

GSClient::GSClient(void)
{
	m_bConnected			= FALSE;
	m_Process				= NULL;
	m_AliveTime				= 0;
	m_WillBeTerminated		= FALSE;
	m_DBStamp				= 0;
	m_DBStampTime			= 0;
	m_PairPlayerId			= 0;
	m_GSServer				= NULL;
	m_DeleteTime			= 0;

	DebugCount.fetch_add(1);
}

boost::shared_ptr<GSPacketTCP>	GSClient::GetTCPSocket()
{
	return m_TCPSocket;
}

boost::shared_ptr<GSPacketUDP>	GSClient::GetUDPSocket()
{
	return m_UDPSocket;
}

GSClient::~GSClient(void)
{
	int count = GetTCPSocket()->m_OLP_REMAIN_COUNT_ACC + GetTCPSocket()->m_OLP_REMAIN_COUNT_REC + GetTCPSocket()->m_OLP_REMAIN_COUNT_SND;
	if (count > 0)
	{
		SYSLOG().Write("GetId() %d alert remain overlap event count acc %d recv %d send %d\n",GetId(), GetTCPSocket()->m_OLP_REMAIN_COUNT_ACC.fetch_add(0), GetTCPSocket()->m_OLP_REMAIN_COUNT_REC.fetch_add(0), GetTCPSocket()->m_OLP_REMAIN_COUNT_SND.fetch_add(0));
	}

	DebugCount.fetch_sub(1);

	GSServer::GSServer* pServer = (GSServer::GSServer*)m_GSServer;

	pServer->GetClientMgr().InsertRecycleId(GetId());
}

BOOL  GSClient::Create(BYTE Type)
{
	CThreadSync Sync;
	
	if(Type == TCP)
	{
		boost::shared_ptr<GSPacketTCP> TCP(new GSPacketTCP);
		m_TCPSocket = TCP;
		m_TCPSocket->m_ClientId = GetId();

		m_CreateType = Type;
	}
	else if(Type == UDP)
	{
		boost::shared_ptr<GSPacketUDP> UDP(new GSPacketUDP);
		m_UDPSocket = UDP;
		m_UDPSocket->m_ClientId = GetId();

		m_CreateType = Type;
	}
	else
		return FALSE;

	return TRUE;
}

INT64 GSClient::GetPair()
{
	return m_PairPlayerId;
}

void GSClient::SetPair(INT64 Player)
{
	m_PairPlayerId = Player;
}

/*BOOL GSClient::Recycle(SOCKET hListenSocket)
{
	CThreadSync Sync;

	if(m_CreateType == TCP)
	{
		m_TCPSocket->Termination();

		return m_TCPSocket->Initialize() && m_TCPSocket->Accept(hListenSocket);
	}
	else if(m_CreateType == UDP)
	{
		m_UDPSocket->Termination();

		return m_UDPSocket->Initialize() && m_UDPSocket->Accept(hListenSocket);
	}

	return FALSE;
}*/

SOCKET GSClient::GetSocket(VOID)
{
	
	if(m_CreateType == TCP)
	{
		return m_TCPSocket->GetSocket();
	}
	else if(m_CreateType == UDP)
	{
		return m_UDPSocket->GetSocket();
	}

	return 0;
}

bool GSClient::GetCompleteJob(const std::wstring str)
{
	CThreadSync Sync;

	std::map<std::wstring,bool>::iterator iter = m_CheckCompleteMap.find(str);
	if(iter == m_CheckCompleteMap.end())
	{
		return false;
	}

	return m_CheckCompleteMap[str];
}

void GSClient::SetCompleteJob(const std::wstring str,bool Value)
{
	CThreadSync Sync;

	m_CheckCompleteMap[str] = Value;
}

BOOL GSClient::InitializeReadForIocp(VOID)
{
	CThreadSync Sync;

	if(m_CreateType == TCP)
	{
		return m_TCPSocket->InitializeReadForIocp();
	}
	else if(m_CreateType == UDP)
	{
		return m_UDPSocket->InitializeReadFromForIocp();
	}

	return FALSE;
}

BOOL GSClient::WriteComplete(VOID)
{
	CThreadSync Sync;

	if(m_CreateType == TCP)
	{
		return m_TCPSocket->WriteComplete();
	}
	else if(m_CreateType == UDP)
	{
		return m_UDPSocket->WriteComplete();
	}

	return FALSE;
}

VOID GSClient::SetConnected(BOOL bConnected)
{
	CThreadSync Sync;

	m_bConnected = bConnected;
}

BOOL GSClient::GetConnected(VOID)
{
	CThreadSync Sync;

	return m_bConnected;
}

boost::shared_ptr<IProcess<int>> GSClient::GetProcess()
{
	return m_Process;
}

void GSClient::SetProcess(boost::shared_ptr<IProcess<int>> Process)
{
	CThreadSync Sync;

	m_Process = Process;

	if(m_Process != NULL)
		m_Process->pOwnerId = GetId();
}

VOID GSClient::OnResponse(LPVOID Data)
{
}

void GSClient::SetAliveTime(DWORD Time)
{
	m_AliveTime = Time;
}

DWORD GSClient::GetAliveTime()
{
	return m_AliveTime;
}

void GSClient::IncDBStamp()
{
	++m_DBStamp;

	m_DBStampTime = GetTickCount();
}

void GSClient::DecDBStamp()
{
	--m_DBStamp;
}

DWORD GSClient::GetDBStamp()
{
	return m_DBStamp;
}

void GSClient::SetWillBeTerminated(bool _terminate)
{
	m_WillBeTerminated = _terminate;
}

BOOL GSClient::GetWillBeTerminated()
{
	return m_WillBeTerminated;
}

VOID GSClient::TakeMsg(boost::shared_ptr<GSClient> client)
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	MSG_PLAYER_PACKET_PTR	pPlayerPacket = ALLOCATOR.Create<MSG_PLAYER_PACKET>();

	pPlayerPacket->pHandler= this;
	pPlayerPacket->Type	= GetMyTP();
	pPlayerPacket->SubType = ONPACKET;
	pPlayerPacket->pClient = client;

	MAINPROC.RegisterCommand(pPlayerPacket);
}

void GSClient::OnEvt(IMessagePtr Arg)
{

}
 
VOID GSClient::Close()
{
	closesocket(GetSocket());
}

BYTE GSClient::GetCreateType()
{
	return m_CreateType;
}

VOID GSClient::ProcPacket(boost::shared_ptr<GSClient> pClient)
{
	CThreadSync Sync;

	boost::shared_ptr<XDATA> pBuffer;

	SetAliveTime(GetTickCount());

	if (GetCreateType() == TCP)
	{
		int size = (int)GetTCPSocket()->m_PacketList.unsafe_size();

		while (GetTCPSocket()->m_PacketList.try_pop(pBuffer) == TRUE)
		{
			size = (int)GetTCPSocket()->m_PacketList.unsafe_size();

			if (pBuffer == NULL) return;

			if (GetProcess() == NULL)
			{
				SYSLOG().Write("set process is null");
				return;
			}

			GetProcess()->Process(pBuffer->m_Buffer.GetBuffer(), pBuffer->Length, pBuffer->MainId, pBuffer->SubId, pClient);
		}
	}
	else if (GetCreateType() == UDP)
	{
		int size = (int)GetUDPSocket()->m_PacketList.unsafe_size();

		while (GetUDPSocket()->m_PacketList.try_pop(pBuffer) == TRUE)
		{
			size = (int)GetUDPSocket()->m_PacketList.unsafe_size();

			if (pBuffer == NULL) return;

			if (GetProcess() == NULL)
			{
				SYSLOG().Write("set process is null\n");

				//echo
				GetUDPSocket()->WriteTo2(const_cast<char *>(pBuffer->LemoteAddress.c_str()), pBuffer->RemotePort, pBuffer->m_Buffer.GetBuffer(), pBuffer->Length);
				return;			
			}

			GetProcess()->Process2(pBuffer->m_Buffer.GetBuffer(), pBuffer->Length, pBuffer->MainId, pBuffer->SubId, pClient, pBuffer->LemoteAddress, pBuffer->RemotePort);
		}
	}

}

VOID GSClient::ProcDisconnect(boost::shared_ptr<GSClient> pClient,bool isForce)
{
	CThreadSync Sync;

	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	//accpet를 안거치고 바로 disconnect되는 애들은 this->GetProcess() null이다 
	//그래서 그넘들은 종료 처리를 해저야하기 때문에 아래에서 리턴시키면 안된다.
	if(GetConnected() == FALSE && isForce == false && this->GetProcess() != NULL)
	{
		SYSLOG().Write("Already Disconnected socket handle %lu getid() %lu \n",(unsigned long)GetSocket(),GetId());
		return ;
	}

	//접속 종료시 로직 쓰레드에 있는 잡은 아래 함수에서 disconnect에서 유저를 빼면 처리가 안될수 있다.
	//iocp worker thread와 로직 쓰레드와 처리가 다르기 때문이다.
	//남은 것을 처리하고 마지막에 종료 시키려면 pServer->Disconnect(pClient); 로직쓰레드로 던져야 한다.
	//
	//소켓을 close 시켜서 OnDisconnect가 떨어져도 player객체가 남아 있는 경우는 
	//소켓을 종료 체크하는 alive 체크 함수 쓰레드와 ProcDisconnect쓰레드가 다른 경우 
	//소켓 멤버를 clear()한 다음 유저 객체 초기화가 화출되어 유저를 못찾았을 가능성이 크다.



	//컨텐츠 종료 처리

	//로직 스레드가 아니라 바로 db tp로 던진다.
	//google auth에서 딜레이가 걸려서 아래 종료 처리 함수가 먼저 실행되면( concurrent 삭제 및 playermgr 제거 ,roommgr 제거)
	//concurrent 함수에서 데이터를 지울수가 없다.
	//그래서 closejob 실행되고 login 함수가 실행되면 순서가 엇갈려 유저가 남게 된다.
	//그러나 완벽하지 않다 logic -> dbthread 2단계이기 때문
	//고민하다가 그냥 logic tp로 던지고 유저 체크 함수를 만듬 (alive check usermgr)

	PROC_REG_CLOSE_JOB(pClient,pServer)


	//상태값 초기화
	SetConnected(FALSE);

	pServer->GetClientMgr().BookDelClient(pClient->GetId());

	pServer->GetClientMgr().NewClient();

}


WORD GSClient::GetMyTP()
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	WORD TP = (GetId() % pServer->GetArgument().m_LogicThreadCnt);
	if(TP < 0)
	{
		SYSLOG().Write("TP is low %d\n",TP);
	}

	return TP;
}

WORD GSClient::GetMyDBTP(WORD StartIndex)
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	return StartIndex + (GetId() % pServer->GetArgument().m_DBThreadCnt);
}

void GSClient::OnSend(WORD MainId,WORD SubId,char *Data,WORD Length)
{

}

void GSClient::OnRecv(DWORD Length, boost::shared_ptr<GSClient> client)
{
	CThreadSync Sync;

	WORD  MainProtocol = 0,SubProtocol = 0;
	DWORD dwPacketLength = 0;

	if (client->GetCreateType() == TCP)
	{
		GetTCPSocket()->MakePacket(Length);

		TakeMsg(client);
	}
	else if (client->GetCreateType() == UDP)
	{
		GetUDPSocket()->MakePacket(Length, MainProtocol, SubProtocol, dwPacketLength);

		//udp의 경우 속도가 중요하고 connectionless이기 때문에 로직 스레드로 던지지 않고 그냥 처리해본다.
		//TakeMsg(client);

		client->ProcPacket(client);
	}
}

void GSClient::OnDisconnect(boost::shared_ptr<GSClient> client, bool isForce)
{
	CThreadSync Sync;

	if(m_TCPSocket->m_Accept_OLP->Overlapped.Internal != 0)
	{
		DWORD Code = GetLastError();
		if(Code == ERROR_NETNAME_DELETED)
		{
			if(m_PairPlayerId != ULONG_MAX)
			{
				SYSLOG().Write("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ERROR_NETNAME_DELETED\n");
			}

			SYSLOG().Write("Code %d ERROR_NETNAME_DELETED\n",Code);

			ProcDisconnect(client,isForce);

			return ;
		}

		SYSLOG().Write("Overlapped.Internal != 0\n");
	}

	//소켓 접속 여유 풀이 없을때 만약 100명이면 100명다 붙어 있을때 클라에서 소켓 연결 요청하면
	//connect에는 콜이 안떨어진다 그 상태에서 접속 종료하면 disconnected만 떨어짐 Connected exception ...2

	if(GetProcess() == NULL)
		SYSLOG().Write("!!!!!!!!!!!!!OnDisconnect get process is nulll\n");

	//GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	//아래 함수를 로직 쓰레드로 던지게 되면 동기화 문제가 발생하여 
	//바로 콜하는 것으로 대체
	//PROC_REG_CLOSE_JOB(this,this,pServer)

	ProcDisconnect(client, isForce);
	
}

void GSClient::OnConnect(boost::shared_ptr<GSClient> pClient)
{
	//Accept가 떨어졌다.
	CThreadSync Sync;

	//SYSLOG().Write("Accept Success Socket %d %d %d\n",GetSocket(),GetId(),GetMyTP());


	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	SetAliveTime(GetTickCount());

	pServer->Accept(pClient);
	pClient->SetConnected(true);

	//아래 함수를 로직 쓰레드로 던지게 되면 동기화 문제가 발생하여 
	//바로 콜하는 것으로 대체


	string ip;
	pClient->GetTCPSocket()->GetLocalIP(ip);

	/*
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	MSG_PLAYER_ACCEPT_PTR pPlayerAccept = ALLOCATOR.Create<MSG_PLAYER_ACCEPT>(); //(new MSG_PLAYER_ACCEPT);// 
	pPlayerAccept->pClient = this;
	pPlayerAccept->pHandler= this;
	pPlayerAccept->Type	= GetMyTP();
	pPlayerAccept->SubType = ONACCEPT;

	MAINPROC.RegisterCommand(pPlayerAccept);
	*/
}

VOID GSClient::Clear()
{
	CThreadSync Sync;

	m_AliveTime = 0;

	m_PairPlayerId  = ULONG_MAX;	
	m_AliveTime = 0;
	m_WillBeTerminated   = FALSE;
	m_DBStamp = 0;
	m_bConnected = FALSE;
	m_DBStampTime = 0;

	m_CheckAbuseMap.clear();
	m_CheckCompleteMap.clear();

	m_DeleteTime = 0;
}

DWORD GSClient::GetDBStampTime()
{
	return m_DBStampTime;
}


//}	}