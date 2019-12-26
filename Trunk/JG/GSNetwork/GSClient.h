#pragma once

#include "GSPacketTCP.h"
#include "GSPacketUDP.h"
#include "IProcess.h"
#include "IResponse.h"
#include "IHandler.h"
#include "./IMessage.h"
#include "IClientSession.h"

#include <boost/pool/object_pool.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace GSNetwork	{	namespace GSClient	{

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

	virtual VOID									RecevieComplete();
	virtual VOID									CloseComplete();
	virtual VOID									AcceptComplete() ;
	virtual VOID									SendComplete() ;

	VOID											Clear();

	BOOL											Recycle(SOCKET hListenSocket);
	SOCKET											GetSocket(VOID);

	boost::shared_ptr<GSPacketTCP>					GetTCPSocket();
	boost::shared_ptr<GSPacketUDP>					GetUDPSocket();

	BOOL											InitializeReadForIocp(VOID);

	VOID											SetConnected(BOOL bConnected) ;
	BOOL											GetConnected(VOID); 

	BOOL											WriteComplete(VOID);

	boost::shared_ptr<IProcess<GSClient>>			GetProcess();
	void											SetProcess(boost::shared_ptr<IProcess<GSClient>> Process);

	virtual VOID									OnResponse(LPVOID Data);
	virtual void									OnEvt(IMessagePtr Arg);

	void											SetAliveTime(DWORD Time);
	DWORD											GetAliveTime();

	void											SetType(BYTE _Type);
	BYTE											GetType();

	void											IncDBStamp();
	void											DecDBStamp();
	DWORD											GetDBStamp();

	void											SetWillBeTerminated(bool _terminate);
	BOOL											GetWillBeTerminated();

	VOID											TakeMsg();

	bool											GetAbuseCheckTime(const std::wstring _str,DWORD _dwTime = 500);

	bool											GetCompleteJob(const std::wstring _str);
	void											SetCompleteJob(const std::wstring _str,bool Value);
	void											ClearCompleteJob();
	DWORD											GetDBStampTime();

public:	

	virtual void									OnSend(WORD MainId,WORD SubId,char *Data,WORD Length);
	virtual void									OnRecv(DWORD Length);
	virtual void									OnDisconnect();
	virtual void									OnConnect();

	VOID											ProcPacket();
	VOID											ProcDisconnect();

	WORD											GetMyTP();
	WORD											GetMyDBTP(WORD StartIndex = 0);

	VOID											Close();

private:

	boost::shared_ptr<IProcess<GSClient>>			m_Process;

	BYTE											m_CreateType;			//tcp,udp
	BOOL											m_bConnected;

	boost::shared_ptr<GSPacketTCP>					m_TCPSocket;
	boost::shared_ptr<GSPacketUDP>					m_UDPSocket;

	BYTE											m_Type;			//server, player

	DWORD											m_AliveTime;

	BOOL											m_WillBeTerminated;
	DWORD											m_DBStamp;
	DWORD											m_DBStampTime;

	std::map<std::wstring,stPacketContext>			m_CheckAbuseMap;
	std::map<std::wstring,bool>						m_CheckCompleteMap;

	DWORD 											m_PairPlayerId;

public:
	LPVOID											m_GSServer;

	DWORD											GetPair();

	void											SetPair(DWORD Player);

	_LogContext										m_LoginContext;
};

#define GSCLIENT  GSNetwork::GSClient::GSClient

#define GSCLIENT_PTR  boost::shared_ptr<GSCLIENT>


}	}