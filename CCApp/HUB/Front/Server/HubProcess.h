#pragma once

class HubProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	HubProcess(void);
	~HubProcess(void);


	virtual VOID Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol,BOOL Compress, boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

protected:

	DECLARE_MSG_MAP(HubProcess)

};

