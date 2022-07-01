#pragma once

////////////////////////////////////////////////////
// Designed for T.C.G by Timothy.
////////////////////////////////////////////////////
//v.1.0

#ifdef _WIN32
	#pragma pack( push ,1 )
#else
	#pragma pack(1)
#endif


#define _PROTOCOL_VERSION 1004

// packet result error codes.
enum ePACKET_RESULT
{
	_ERR_NONE = 0,		// Error is not occured.
	_ERR_NOTREGISTERED,	// Not registered User.
	_ERR_PWD,			// Wrong Password.
	_ERR_NETWORK,		// User socket error occured.
	_ERR_PROTOCOL,		// denied for invalid protocol version
	_ERR_OCCUR,			// 에러발생
	_ERR_DISABLE,		// 불가능
	_ERR_LOGINED,		// 로그인시 이미 로그인 되었을때
	_ERR_WRONG_CHAR_NAME,	//잘못된 캐릭터이름
	_ERR_TOO_MANY_USERS		//유저가 이미 풀입니다.
};

// common packet data
typedef struct PACKET_BASE
{
	unsigned short	MPID;	// Main Packet ID
	unsigned short	SPID;	// Sub Packet ID
	unsigned short	Len;	// Packet Size
	unsigned short	FROM;	// Purpose of Packet 
} _PKT_BASE;

enum eLB_MAIN_PACKET_ID
{
	_GS_PKT_LOGIN = 1,					/// 게임서버 로그인
	_GS_PKT_LOGIN_RE,					/// 게임서버 로그인 응답
	_GS_PKT_LOGOUT,						/// 게임서버 로그아웃
	_GS_PKT_UPDATE_DATA,
	_GS_PKT_UPDATE_DATA_RE,
	_GS_PKT_REQ_DATA,
	_GS_PKT_REQ_DATA_RE,

	_GS_PKT_MAX,						// 항상 마지막에 둔다 이 값으로 현재 패킷의 개수 와 패킷의 유효성을 검증한다.
};

//Client->Server
struct LS_PKT_LOGIN : PACKET_BASE			/// 로그인 요청
{
	char	szUserID[_MAX_USER_ID_SIZE];	/// 아이디
	char	szPwd[_MAX_PWD_SIZE];			/// 패스워드
	LS_PKT_LOGIN() {
		memset(this, 0x00, sizeof(LS_PKT_LOGIN));
		MPID = _GS_PKT_LOGIN;
		Len = sizeof(LS_PKT_LOGIN);
	};
};

//Server->Client
struct LS_PKT_LOGIN_RE : PACKET_BASE				///로그인 요청 결과
{
	WORD		wResult;							/*_ERR_NONE = 0,		 // Error is not occured.
													 _ERR_NOTREGISTERED,	 // Not registered User.
													 _ERR_PWD,				 // Wrong Password.
													 _ERR_NETWORK,			 // User socket error occured.
													 _ERR_PROTOCOL,			 // denied for invalid protocol version
													 _ERR_OCCUR,			 // 에러발생
													 _ERR_DISABLE,			 // 불가능
													 _ERR_LOGINED,			 // 로그인시 이미 로그인 되었을때
													 _ERR_WRONG_CHAR_NAME	 //잘못된 캐릭터이름"
													*/
												
	LS_PKT_LOGIN_RE() {
		memset(this, 0x00, sizeof(LS_PKT_LOGIN_RE));
		MPID = _GS_PKT_LOGIN_RE;
		Len = sizeof(LS_PKT_LOGIN_RE);
	};
};


struct GS_PKT_UPDATE_DATA : PACKET_BASE
{
	char	szUserID[_MAX_USER_ID_SIZE];	/// 아이디
	char	szPwd[_MAX_PWD_SIZE];			/// 패스워드
	char	Data[512];

	GS_PKT_UPDATE_DATA()
	{
		memset(this, 0x00, sizeof(GS_PKT_UPDATE_DATA));
		MPID = _GS_PKT_UPDATE_DATA;
		Len = sizeof(GS_PKT_UPDATE_DATA);
	}
};

struct GS_PKT_UPDATE_DATA_RE : PACKET_BASE
{
	WORD Result;		//0성공

	GS_PKT_UPDATE_DATA_RE()
	{
		memset(this, 0x00, sizeof(GS_PKT_UPDATE_DATA_RE));
		MPID = _GS_PKT_UPDATE_DATA_RE;
		Len = sizeof(GS_PKT_UPDATE_DATA_RE);
	}
};
struct GS_PKT_REQ_DATA : PACKET_BASE
{
	char	szUserID[_MAX_USER_ID_SIZE];	/// 아이디
	char	szPwd[_MAX_PWD_SIZE];			/// 패스워드
	
	GS_PKT_REQ_DATA()
	{
		memset(this, 0x00, sizeof(GS_PKT_REQ_DATA));
		MPID = _GS_PKT_REQ_DATA;
		Len = sizeof(GS_PKT_REQ_DATA);
	}
};

struct GS_PKT_REQ_DATA_RE : PACKET_BASE
{
	char	Data[512];
	WORD Result;		//0성공

	GS_PKT_REQ_DATA_RE()
	{
		memset(this, 0x00, sizeof(GS_PKT_REQ_DATA_RE));
		MPID = _GS_PKT_REQ_DATA_RE;
		Len = sizeof(GS_PKT_REQ_DATA_RE);
	}
};


///////////////////////////////////////////////////////////////////////
#pragma pack()
