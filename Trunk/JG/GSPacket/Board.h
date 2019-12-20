#pragma once
 #pragma pack(push, 1) 

#include "stdafx.h"
#include "proto_header.h"

typedef enum _BOARD_PROTOCOL
{
	
	ID_PKT_LOGIN_REQ,					//클라이언트 인증 요청
	ID_PKT_LOGIN_RES,					//클라이언트 인증 결과

	ID_PKT_CREATE_ROOM_REQ,				//방 생성 요청
	ID_PKT_CREATE_ROOM_RES,				//방 생성 요청 결과

	ID_PKT_LEAVE_ROOM_REQ,				//방 퇴장 요청
	ID_PKT_LEAVE_ROOM_RES,				//방 퇴장 요청 결과

	ID_PKT_ENTER_ROOM_REQ,				//방 입장 요청
	ID_PKT_ENTER_ROOM_RES,				//방 입장 요청 결과

	ID_PKT_NEW_USER_IN_ROOM,				//방 입장 유저 알림

	ID_PKT_CHAT_ROOM_REQ,				//방 채팅 요청z
	ID_PKT_CHAT_ROOM_RES,				//방 채팅 요청 결과

	ID_PKT_READY_ROOM_REQ,				//플레이어 레디 상태 요청
	ID_PKT_READY_ROOM_RES,				//플레이어 레디 결과

	ID_PKT_START_ROOM_REQ,				//방 게임 시작 상태 요청
	ID_PKT_START_ROOM_RES,				//방 게임 시작  결과

	ID_PKT_RESULT_ROOM,					//방 게임  결과
	
	ID_PKT_ROOM_LIST_REQ,				//방 리스트 요청
	ID_PKT_ROOM_LIST_RES,				//방 리스트 요청

	ID_PKT_USER_LIST_REQ,				//유저 리스트 요청
	ID_PKT_USER_LIST_RES,				//유저 리스트 요청

	ID_PKT_ALL_COMPLETE_NTY,				//준비 완료
	ID_PKT_ALL_COMPLETE,					//모두 준비완료

	ID_PKT_AUTO_START_REQ,

	ID_HELLO_WORLD,						//로비 입장 성공
};



#pragma pack(pop)
	
	

	
	

	
	

	
	

			
