#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "IPlayerProcess.h"

class IUdpProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	IUdpProcess(void);
	~IUdpProcess(void);

	VOID Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient) {}
	VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport);

	
	VOID ENTER_ROOM(LPVOID Data,DWORD Length, boost::shared_ptr<GSClient> pClient);
	//VOID CLOSE_RELAYSVR(LPVOID Data,DWORD Length);

	//VOID BROADCAST_NTY(LPVOID Data,DWORD Length);

	//VOID CREATE_UDP_SOCKET_NTY(LPVOID Data,DWORD Length);


protected:

	DECLARE_MSG_MAP(IUdpProcess)
};
