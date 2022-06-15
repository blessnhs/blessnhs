#pragma once

class HubProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	HubProcess(void);
	~HubProcess(void);


	virtual VOID Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, BOOL Compress,boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

	VOID VERSION(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID LOGIN_PLAYER(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_CREATE(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_ENTER(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_LEAVE(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_START(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_READY(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_BITMAP_CHAT(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_AUDIO_CHAT(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ALL_COMPLETE(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID AUTO_START(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_LIST(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID MATCH(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID RANK(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_PASSTHROUGH(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID QNS(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID PRAY_LIST(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID REG_PRAY(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID NOTICE(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID CHECK_NICKNAME(WORD SubProtocol,LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID LOGOUT_CLIENT(WORD SubProtocol, LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID MPEG2TS_MESSAGE(WORD SubProtocol, LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID MPEG2TS_WAKE_UP(WORD SubProtocol, LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

protected:

	DECLARE_MSG_MAP2(HubProcess)

};

