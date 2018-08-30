#pragma once

#pragma pack( push ,1 )


////////////////////////////////////////////////////////////////////////////
typedef enum _PROTOCOL_DBA
{
	// 프로토콜의 시작을 알립니다.
	PT_VERSION_DBA	= 1,

	DBP_GET_LOGIN_INFO,
	DBP_GET_LOGIN_INFO_RE,
	DBP_GET_CHAR_INFO,
	DBP_GET_CHAR_INFO_RE,

	DBP_UPDATE_CHAR_INFO,
	DBP_UPDATE_CHAR_INFO_RE,

	DBP_REQUEST_CHANNEL_LIST,
	DBP_REQUEST_CHANNEL_LIST_RE,

	DBP_REQUEST_CLOSE_PLAYER,
	DBP_REQUEST_CLEAR_CONCURRENT,
};

////////////////////////////////////////////////////////////////////////////

#pragma pack()
