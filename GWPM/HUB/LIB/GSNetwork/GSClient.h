#pragma once

#include <boost/enable_shared_from_this.hpp>

#include "GSPacketTCP.h"
#include "GSPacketUDP.h"
#include "IProcess.h"
#include "IResponse.h"
#include "IHandler.h"
#include "./IMessage.h"
#include "IClientSession.h"

using namespace GSNetwork;
//namespace GSNetwork	{	namespace GSClient	{

typedef GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;
typedef GSSocket::GSPacket::GSPacketUDP::GSPacketUDP GSPacketUDP;

//패킷 필터링 관련 정보 구조체
struct stPacketContext
{
	DWORD m_dwTime;
	BOOL  m_bUse;
};

struct _LogContext
{
	WCHAR account[MAX_ID_LENGTH];
	bool IsSendedAuth;
	DWORD Time;
};

class GSClient : public GSGuard<GSClient>,public IClientSession::IClientSession ,public IResponse , public IHandler<IMessagePtr>, public boost::enable_shared_from_this<GSClient>
{
public:

	GSClient(void);
	~GSClient(void);

	BOOL											Create(BYTE Type);

	VOID											Clear();


//	BOOL											Recycle(SOCKET hListenSocket);
	SOCKET											GetSocket(VOID);

	boost::shared_ptr<GSPacketTCP>					GetTCPSocket();
	boost::shared_ptr<GSPacketUDP>					GetUDPSocket();

	BOOL											InitializeReadForIocp(VOID);

	VOID											SetConnected(BOOL bConnected) ;
	BOOL											GetConnected(VOID); 

	BOOL											WriteComplete(VOID);

	boost::shared_ptr<IProcess<int>>				GetProcess();
	void											SetProcess(boost::shared_ptr<IProcess<int>> Process);

	template<class U>
	VOID   										    SetHandler()
	{
		
		boost::shared_ptr<U> Handler(new U);
		SetProcess(Handler);
	}

	virtual VOID									OnResponse(LPVOID Data);
	virtual void									OnEvt(IMessagePtr Arg);

	void											SetAliveTime(DWORD Time);
	DWORD											GetAliveTime();

	void											IncDBStamp();
	void											DecDBStamp();
	DWORD											GetDBStamp();

	void											SetWillBeTerminated(bool _terminate);
	BOOL											GetWillBeTerminated();

	VOID											TakeMsg(boost::shared_ptr<GSClient> client);


	bool											GetCompleteJob(const std::wstring _str);
	void											SetCompleteJob(const std::wstring _str,bool Value);
	DWORD											GetDBStampTime();

public:	

	virtual void									OnSend(WORD MainId,WORD SubId,char *Data,WORD Length);
	virtual void									OnRecv(DWORD Length, boost::shared_ptr<GSClient> client);
	virtual void									OnDisconnect(boost::shared_ptr<GSClient> client, bool isForce = false);
	virtual void									OnConnect(boost::shared_ptr<GSClient> client);

	VOID											ProcPacket(boost::shared_ptr<GSClient> pClient);
	VOID											ProcDisconnect(boost::shared_ptr<GSClient> pClient,bool isForce = false);

	WORD											GetMyTP();
	WORD											GetMyDBTP(WORD StartIndex = MSG_TYPE_DB_1);

	VOID											Close();

	BYTE											GetCreateType();

private:


	boost::shared_ptr<IProcess<int>>				m_Process;

	BYTE											m_CreateType;			//tcp,udp
	BOOL											m_bConnected;

	boost::shared_ptr<GSPacketTCP>					m_TCPSocket;
	boost::shared_ptr<GSPacketUDP>					m_UDPSocket;

	DWORD											m_AliveTime;

	BOOL											m_WillBeTerminated;
	DWORD											m_DBStamp;
	DWORD											m_DBStampTime;

	std::map<std::wstring,stPacketContext>			m_CheckAbuseMap;
	std::map<std::wstring,bool>						m_CheckCompleteMap;

	INT64 											m_PairPlayerId;

public:
	LPVOID											m_GSServer;

	INT64											GetPair();

	void											SetPair(INT64 Player);

	//내가 삭제될 타임 종료후 iocp 메세지가 올수 있으므로 대기 했다가 제거
	DWORD											m_DeleteTime;

};

#define GSCLIENT  GSClient

#define GSCLIENT_PTR  boost::shared_ptr<GSCLIENT>


//}	}