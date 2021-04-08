#include "StdAfx.h"
#include "GSClientSocket.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/Relay.h"
#include "CSV.h"
#include "CSVDlg.h"
#include "LOBBY.h"
#include "ROOM.h"

extern CCSVDlg dlg;

extern DWORD MyIndex;

GSClientSocket::GSClientSocket(void)
{
	Position = 0;

	

//	SendMessage(WM_SYSCOMMAND,20000,0);

}


GSClientSocket::~GSClientSocket(void)
{
}

VOID	GSClientSocket::OnIoRead(VOID)
{
	GSClientModule::OnIoRead();
}

VOID	GSClientSocket::OnIoConnected(VOID)
{
	if(Position == 0)		//인증
	{
	}
	else if(Position == 1)  //로비
	{
/*		CF_PKT_REGIST_REQ req;

		req.Id			= id;
		req.SessionKey  = sessionkey;
	
		Send(req);
*/
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		Json::Value root2;
		root2["MPID"]   = ID_CF_PKT_REGIST_REQ;

		CF_PKT_REGIST_REQ SendData;


		std::string Id_;
		std::string key_;

		sessionkey;

		Id_.assign(id.begin(),id.end());
		key_.assign(sessionkey.begin(),sessionkey.end());
		
		
		char Key[1024];
		wcstombs(Key,sessionkey.c_str(),1000);


		root2["Result"] = 0;
		root2["Id"] = Id_.c_str();
		root2["SessionKey"]	 = key_.c_str();

		Json::FastWriter writer;
		std::string outputConfig = writer.write( root2 );

		WritePacket(ID_CF_PKT_REGIST_REQ,0,(BYTE*)outputConfig.c_str	(),outputConfig.size());

	}
	else if(Position == 2)	// 게임방 릴레이
	{
		Json::Value root2;
		root2["MPID"]   = ID_CR_ENTER_ROOM_REQ;

		CF_PKT_REGIST_REQ SendData;

		root2["Id"] = 1;
		root2["PlayerIndex"]	 = (int)MyIndex;

		Json::FastWriter writer;
		std::string outputConfig = writer.write( root2 );

		WritePacket(ID_CR_ENTER_ROOM_REQ,0,(BYTE*)outputConfig.c_str	(),outputConfig.size());

	}
}

VOID	GSClientSocket::OnIoDisconnected(VOID)
{

}

