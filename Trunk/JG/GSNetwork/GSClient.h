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

using namespace GSNetwork;
//namespace GSNetwork	{	namespace GSClient	{

typedef GSSocket::GSPacket::GSPacketTCP::GSPacketTCP GSPacketTCP;
typedef GSSocket::GSPacket::GSPacketUDP::GSPacketUDP GSPacketUDP;

//��Ŷ ���͸� ���� ���� ����ü
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

	void											SetType(BYTE _Type);
	BYTE											GetType();

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
	virtual void									OnDisconnect(boost::shared_ptr<GSClient> client);
	virtual void									OnConnect(boost::shared_ptr<GSClient> client);

	VOID											ProcPacket(boost::shared_ptr<GSClient> pClient);
	VOID											ProcDisconnect(boost::shared_ptr<GSClient> pClient);

	WORD											GetMyTP();
	WORD											GetMyDBTP(WORD StartIndex = MSG_TYPE_DB_1);

	VOID											Close();

private:


	boost::shared_ptr<IProcess<int>>				m_Process;

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

	//���� ������ Ÿ�� ������ iocp �޼����� �ü� �����Ƿ� ��� �ߴٰ� ����
	DWORD											m_DeleteTime;

};

#define GSCLIENT  GSClient

#define GSCLIENT_PTR  boost::shared_ptr<GSCLIENT>


//}	}