// ROOM.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CSV.h"
#include "ROOM.h"
#include "afxdialogex.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/Relay.h"
#include "GSClientSocket.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSSerialize.h"
#include "UDPSession.h"

extern std::vector<UDP_Client*> udp_client;

// ROOM 대화 상자입니다.

IMPLEMENT_DYNAMIC(ROOM, CDialogEx)

ROOM::ROOM(CWnd* pParent /*=NULL*/)
	: CDialogEx(ROOM::IDD, pParent)
{
	
}

ROOM::~ROOM()
{
}

BOOL ROOM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//	p_login = new login;

	//	p_login->ShowWindow(SM_SHOWSOUNDS);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetTimer(0,10,NULL);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void ROOM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void ROOM::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

	}
	else
	{
		CDialogEx::OnPaint();
	}


}


BEGIN_MESSAGE_MAP(ROOM, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE, &ROOM::OnBnClickedButtonLeave)
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON_STUN2, &ROOM::OnBnClickedButtonStun2)
	ON_BN_CLICKED(IDC_BUTTON_STUN3, &ROOM::OnBnClickedButtonStun3)
	ON_LBN_SELCHANGE(IDC_LIST_CHAT, &ROOM::OnLbnSelchangeListChat)
END_MESSAGE_MAP()


// ROOM 메시지 처리기입니다.


void ROOM::OnBnClickedButtonLeave()
{
//	CF_PKT_LEAVE_ROOM_REQ snd;
//	snd.RoomNumber = RoomNumber;
//	m_SocketLobby.Send(snd);

	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("RoomNumber",(WORD)RoomNumber)
	
	Json::FastWriter writer;	
	std::string outputConfig = writer.write( root2 );	
	m_SocketLobby.WritePacket(ID_CF_PKT_LEAVE_ROOM_REQ,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 
}


void ROOM::OnTimer(UINT_PTR nIDEvent)
{
		// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_SocketLogin.Proc();
	m_SocketLobby.Proc();
	m_SocketRelay.Proc();

	for(int i=0;i<udp_client.size();i++)
	{
		if(udp_client[i])
		{
			udp_client[i]->Proc();
		}
	}	

	CDialogEx::OnTimer(nIDEvent);
}

BOOL ROOM::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch( pMsg->wParam )
		{
		case VK_RETURN:
		case VK_ESCAPE:
			{

				CString msg;
				GetDlgItem(IDC_EDIT1)->GetWindowText(msg);

				CF_PKT_CHAT_ROOM_REQ snd;

				snd.Chat = msg.GetBuffer(msg.GetLength());


				DECLARE_JSON_WRITER
				ADD_JSON_WSTR_MEMBER("Chat",snd.Chat)

				Json::FastWriter writer;	
				std::string outputConfig = writer.write( root2 );	
				m_SocketLobby.WritePacket(ID_CF_PKT_CHAT_ROOM_REQ,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 

			//	m_SocketLobby.Send(snd);

				GetDlgItem(IDC_EDIT1)->SetWindowText(_T(""));

				return CWnd::PreTranslateMessage(pMsg);
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg); 
}


void ROOM::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void ROOM::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch(nID)
	{
	case 9000:
		OnClose();
		OnOK();
		break;
	}

	CDialogEx::OnSysCommand(nID, lParam);
}


void ROOM::OnBnClickedButtonStun2()
{
	m_SocketRelay.Position = 2;

	printf("MyIndex %d\n",MyIndex);

	m_SocketRelay.BeginTcp("127.0.0.1",8577);
}

void ROOM::OnBnClickedButtonStun3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ROOM::OnLbnSelchangeListChat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
