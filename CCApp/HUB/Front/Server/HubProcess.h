#pragma once

class HubProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	HubProcess(void);
	~HubProcess(void);


	virtual VOID Process(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

protected:

	DECLARE_MSG_MAP(HubProcess)

};

