#pragma once

#define MAX_BUFFER_LENGTH		1024 * 5
#define MAX_ID_LENGTH			50
#define MAX_CHAR_LENGTH			512
#define MAX_CHAR_CNT			5
#define MAX_PASSWD_LENGTH		20
#define MAX_SESSION_KEY_SIZE	64
#define MAX_INI_STRING_LENGTH	256
#define SERVER_NAME				256

#define MAX_ROOM_NAME			56
#define MAX_ROOM_PASSWD			24

#define MAX_ROOM_CHAT_SIZE		256 
#define MAX_ROOM_LIST_TRNS      20

#define MAX_ROOM_PLAYER			32

#define MAX_IP_ADDRESS			20	
#define MAX_CHANNEL_NAME_LEGNTH 50

#define MAX_CHANNEL_COUNT       255

#define MAX_CHANNEL_NAME		256

#define MAX_LOBBY_CHAT			256

#define MAX_UDP_SOCKET_COUNT	2000
		


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


enum TEAM
{
	RED		=	0,
	BLUE	=	1,

};

typedef char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long sl32;
typedef unsigned long ul32;
typedef signed __int64 s64;
typedef unsigned __int64 u64;

#define	MAX_TRNASMISSION_COUNT	20

enum EVT_TYPE
{
	MSG_TYPE_USER = 0,
	MSG_TYPE_DB_1 = 2500,
	MSG_TYPE_LOG  = 3500,
};

#define GT(X,MAX) (X%MAX)

enum EVT_SUB_TYPE
{
	ONACCEPT=0,
	ONPACKET,
	ONCLOSE,
	ONQUERY,
	ONQUERY_RES,
	ONTIME,

};

enum 
{
	TCP = 0,
	UDP
};
enum _eType
{
	_PLAYER_=1,
	_SERVER_,
};


enum SERVER_TYPE
{
	FRONT_SERVER = 0,
};

enum USER_EVNT_TYPE
{
	ON_ACCEPT=0,
	ON_PACKET,
	ON_CLOSE,
	ON_QUERY,
	ON_TIME,
	ON_QUERY_RES,
};