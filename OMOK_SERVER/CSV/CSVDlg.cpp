
// CSVDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "CSV.h"
#include "CSVDlg.h"
#include "afxdialogex.h"
#include "GSClientSocket.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/auth.h"

#include "LOBBY.h"
#include "ROOM.h"
#include "UDPSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};



CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCSVDlg 대화 상자




CCSVDlg::CCSVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCSVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCSVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCSVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCSVDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCSVDlg 메시지 처리기

BOOL CCSVDlg::OnInitDialog()
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	m_SocketLogin.BeginTcp("127.0.0.1",8599);	

	SetTimer(0,10,NULL);
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BOOL CCSVDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg); 
}

void CCSVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
 	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCSVDlg::OnPaint()
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

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCSVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCSVDlg::OnBnClickedOk()
{

	char posbuff[10];

	sprintf(posbuff	,"\"%d\"",0);

	CString ID,PWD;

	GetDlgItemText(IDC_LOGIN_EDIT1,ID);
	GetDlgItemText(IDC_LOGIN_EDIT2,PWD);

	Json::Value root2;
	root2["MPID"]   = ID_AUTH_LOGIN_REQ;

	PKT_AUTHENCATION_RES SendData;

	wstring id = ID;
	wstring pwd= PWD;

	std::string _Id;
	std::string _Passwd;

	_Id.assign(id.begin(),id.end());
	_Passwd.assign(pwd.begin(),pwd.end());

	root2["Id"] = _Id.c_str();
	root2["Passwd"]	 = _Passwd.c_str();

	Json::FastWriter writer;
	std::string outputConfig = writer.write( root2 );

	m_SocketLogin.WritePacket(ID_AUTH_LOGIN_REQ,0,(BYTE*)outputConfig.c_str(),outputConfig.size());

	
	
	{

	Json::Value		root2;	
	Json::Value		value;	
	Json::Value		array;	

	std::string Data = "{\"room\":[{\"Index\":5,\"Name\":\"TEST\"}]}";

	Json::Reader	reader;	
	bool bParse = reader.parse((char *)Data.c_str(), root2);	
	if(!bParse) {	
		printf("Json String Parse Error...%s\r\n",Data.c_str());	
		return;	
	}	


	bool d = root2.isArray();

	int asize = root2["room"].size();

	const Json::Value& books = root2["room"];
	for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
	{
		const Json::Value& book = *it;

		int Index = book["Index"].asInt();
		string name = book["Name"].asString();
		// rest as before
	}


	}




/*	PKT_AUTHENCATION_REQ req;

	req.Id = ID;
	req.Passwd = PWD;

	m_SocketLogin.Send(req);*/
} 


void CCSVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	CDialogEx::OnTimer(nIDEvent);
}
