#pragma once

#include "IPlayerProcess.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

class HubProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	HubProcess(void);
	~HubProcess(void);


	virtual VOID Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

	VOID VERSION(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID LOGIN_PLAYER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_CREATE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_ENTER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_LEAVE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_READY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_BITMAP_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_AUDIO_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ALL_COMPLETE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID AUTO_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID MATCH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID RANK(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_PASSTHROUGH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID QNS(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID PRAY_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID REG_PRAY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID NOTICE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID CHECK_NICKNAME(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

protected:

	DECLARE_MSG_MAP(HubProcess)

};

