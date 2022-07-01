#pragma once


// game_room 대화 상자입니다.

class game_room : public CDialogEx
{
	DECLARE_DYNAMIC(game_room)

public:
	game_room(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~game_room();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHAT_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStun();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStun2();
};
