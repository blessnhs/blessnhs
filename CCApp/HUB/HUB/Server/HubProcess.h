#pragma once

class HubProcess : public GSNetwork::IPlayerProcess::IPlayerProcess
{
public:
	HubProcess(void);
	~HubProcess(void);


	virtual VOID Process(boost::shared_ptr<XDATA> pBuffer,boost::shared_ptr<GSClient> Client);
	virtual VOID Process2(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> pClient, string remoteaddress, int remoteport) {}

	VOID VERSION(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID LOGIN_PLAYER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID ROOM_CREATE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_ENTER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_LEAVE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_START(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_READY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_BITMAP_CHAT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID ROOM_AUDIO_CHAT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID ALL_COMPLETE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID AUTO_START(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID ROOM_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID MATCH(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID RANK(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID ROOM_PASSTHROUGH(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID QNS(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID PRAY_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);
	VOID REG_PRAY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID NOTICE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CHECK_NICKNAME(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID LOGOUT_CLIENT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID MPEG2TS_MESSAGE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CAMERA_WAKE_UP(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CAMERA_LIST(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CAMERA_REGISTER(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CAMERA_DELETE(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID STOP_STREAM(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID CAMERA_CONTROL(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID MACHINE_STATUS(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID PURCHASE_VERIFY(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

	VOID RECONNECT(boost::shared_ptr<XDATA> pBuffer, boost::shared_ptr<GSClient> Client);

protected:

	DECLARE_MSG_MAP2(HubProcess)

};

