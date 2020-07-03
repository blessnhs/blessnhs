#pragma once

#include "IPlayerProcess.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

#include <boost/shared_ptr.hpp>

class BoardProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	BoardProcess(void);
	~BoardProcess(void);


	virtual VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol, boost::shared_ptr<GSClient> Client);

	VOID VERSION(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID LOGIN_PLAYER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_CREATE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_ENTER(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_LEAVE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_READY(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ROOM_CHAT(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);
	VOID ALL_COMPLETE(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID AUTO_START(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_LIST(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID MATCH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID RANK(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID ROOM_PASSTHROUGH(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

	VOID QNS(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> Client);

protected:

	DECLARE_MSG_MAP(BoardProcess)

};

