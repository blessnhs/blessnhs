#pragma once

#include "IProcess.h"
#include "MessageMapDef.h"
#include "IPlayerProcess.h"

class IUdpProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	IUdpProcess(void);
	~IUdpProcess(void);

	VOID Process(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> pClient) {}
	VOID Process2(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport);

	VOID REG_USER_UDP(LPVOID Data,DWORD Length, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport);
	VOID BROAD_CAST_ROOM_UDP(LPVOID Data, DWORD Length, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport);
	//VOID CLOSE_RELAYSVR(LPVOID Data,DWORD Length);

	//VOID BROADCAST_NTY(LPVOID Data,DWORD Length);

	//VOID CREATE_UDP_SOCKET_NTY(LPVOID Data,DWORD Length);


protected:

	DECLARE_MSG_MAP(IUdpProcess)
};

