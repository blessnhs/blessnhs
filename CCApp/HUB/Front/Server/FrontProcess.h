#pragma once

class FrontProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	FrontProcess(void);
	~FrontProcess(void);


	virtual VOID Process(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol,BOOL IsCompress, boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(LPVOID Data, DWORD Length, WORD MainProtocol, WORD SubProtocol, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

protected:

	DECLARE_MSG_MAP(FrontProcess)

};

