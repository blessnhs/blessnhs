#pragma once


// ROOM 대화 상자입니다.

class ROOM : public CDialogEx
{
	DECLARE_DYNAMIC(ROOM)

public:
	ROOM(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ROOM();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnPaint();

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLeave();

	DWORD RoomNumber;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonStun2();
	
	afx_msg void OnBnClickedButtonStun3();
	afx_msg void OnLbnSelchangeListChat();
};
