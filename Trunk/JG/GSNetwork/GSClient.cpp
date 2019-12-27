#include "StdAfx.h"
#include "GSClient.h"
#include "GSMainProcess.h"
#include "MSG_PLAYER_CLOSE.h"
#include "MSG_PLAYER_ACCEPT.h"
#include "MSG_PLAYER_PACKET.h"

#include "GSAllocator.h"
#include "GSServer.h"

//namespace GSNetwork	{	namespace GSClient	{

GSClient::GSClient(void)
{
	m_bConnected			= FALSE;
	m_Process				= NULL;
	m_AliveTime				= 0;
	m_WillBeTerminated		= FALSE;
	m_DBStamp				= 0;
	m_DBStampTime			= 0;
	m_PairPlayerId			= ULONG_MAX;
	m_GSServer				= NULL;

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

DWORD GSClient::GetPair()
{
	return m_PairPlayerId;
}

void GSClient::SetPair(DWORD Player)
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

void GSClient::SetType(BYTE _Type)
{
	m_Type = _Type;
}

BYTE GSClient::GetType()
{
	return m_Type;
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

VOID GSClient::ProcPacket(boost::shared_ptr<GSClient> pClient)
{
	CThreadSync Sync;

	boost::shared_ptr<XDATA> pBuffer;

	SetAliveTime(GetTickCount());

	int size = GetTCPSocket()->m_PacketList.unsafe_size();

	while(GetTCPSocket()->m_PacketList.try_pop(pBuffer) == TRUE)
	{
		size = (int)GetTCPSocket()->m_PacketList.unsafe_size();

		if(pBuffer == NULL) return ;

		if(GetProcess() == NULL)
		{
			printf("set process is null");
			return ;
		}

		GetProcess()->Process(pBuffer->m_Buffer.GetBuffer(),pBuffer->Length,pBuffer->MainId,pBuffer->SubId, pClient);
	}
}

VOID GSClient::ProcDisconnect(boost::shared_ptr<GSClient> pClient)
{
	CThreadSync Sync;
	
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	if(GetConnected() == FALSE)
	{
		printf("Already Disconnected socket  %d %d\n",GetSocket(),GetId());
		return ;
	}

	if(pServer->Disconnect(pClient) == TRUE)
	{
		SetConnected(FALSE);
		Clear();
//		BOOL Result = Recycle(pServer->GetTcpListen()->GetSocket());

		pServer->GetClientMgr().DelClient(pClient->GetId());

		int NewClient = 1;
		if (pServer->GetClientMgr().GetActiveSocketCount() < 20)
		{
			printf("New Alloc  %d \n", NewClient);
			NewClient = 100;
		}

		pServer->GetClientMgr().NewClient(pServer->GetTcpListen()->GetSocket(), NewClient, pServer);
	}
	else
	{
		printf("Wait Cant Disconnected socket  %d %d\n",GetSocket(),pServer->GetTcpListen()->GetSocket());	
	}
}


WORD GSClient::GetMyTP()
{
	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	WORD TP = (GetId() % pServer->GetArgument().m_LogicThreadCnt);
	if(TP < 0)
	{
		printf("TP is low %d\n",TP);
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

	GetTCPSocket()->MakePacket(Length,MainProtocol,SubProtocol,dwPacketLength);

	TakeMsg(client);
}

void GSClient::OnDisconnect(boost::shared_ptr<GSClient> client)
{
	CThreadSync Sync;

	if(m_TCPSocket->m_Accept_OLP.Overlapped.Internal != 0)
	{
		DWORD Code = GetLastError();
		if(Code == ERROR_NETNAME_DELETED)
		{
			if(m_PairPlayerId != ULONG_MAX)
			{
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ERROR_NETNAME_DELETED\n");
			}

			printf("Code %d ERROR_NETNAME_DELETED\n",Code);

			ProcDisconnect(client);

//			Clear();
//			SetConnected(FALSE);
//			GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;
//			Recycle(pServer->GetTcpListen()->GetSocket());
			return ;
		}
	}

	//GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	//아래 함수를 로직 쓰레드로 던지게 되면 동기화 문제가 발생하여 
	//바로 콜하는 것으로 대체
	//PROC_REG_CLOSE_JOB(this,this,pServer)

	ProcDisconnect(client);
	
}

void GSClient::OnConnect(boost::shared_ptr<GSClient> pClient)
{
	//Accept가 떨어졌다.
	CThreadSync Sync;

	SetConnected(TRUE);
	//printf("Accept Success Socket %d %d %d\n",GetSocket(),GetId(),GetMyTP());

	//아래 함수를 로직 쓰레드로 던지게 되면 동기화 문제가 발생하여 
	//바로 콜하는 것으로 대체

	GSServer::GSServer *pServer = (GSServer::GSServer *)m_GSServer;

	SetAliveTime(GetTickCount());

	pServer->Accept(pClient);

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

VOID GSClient::RecevieComplete()
{

}

VOID GSClient::CloseComplete()
{

}

VOID GSClient::AcceptComplete()
{

}

VOID GSClient::SendComplete()
{

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

	m_LoginContext.IsSendedAuth = false;
	m_LoginContext.account[0] = NULL;
	m_LoginContext.Time = 0;
}

DWORD GSClient::GetDBStampTime()
{
	return m_DBStampTime;
}


//}	}