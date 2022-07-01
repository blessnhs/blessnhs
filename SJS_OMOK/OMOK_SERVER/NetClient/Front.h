#pragma once

#include "stdafx.h"
#include "common.h"
#include "proto_header.h"

template<typename Archive>
void serialize(Archive& ar,RoomInfo &g,const unsigned int version)
{
	ar & g.Index;
	ar & g.Name;
}

template<typename Archive>
void serialize(Archive& ar,RoomPlayerStock &g,const unsigned int version)
{
	ar & g.Name;
	ar & g.SeatPos;
}

template<typename Archive>
void serialize(Archive& ar,UserInfo &g,const unsigned int version)
{
	ar & g.Name;
}

typedef enum _FRONT_PROTOCOL
{
	ID_GF_PKT_REGIST_REQ = 1000,			//인증 서버
	ID_FG_PKT_REGIST_RES,					//인증 서버 결과
	
	ID_CF_PKT_REGIST_REQ,					//클라이언트 인증 요청
	ID_FC_PKT_REGIST_RES,					//클라이언트 인증 결과

	ID_CF_PKT_CLOSE_REQ,					//클라이언트 접속 종료 요청
	ID_CF_PKT_CLOSE_RES,					//클라이언트 접속 종료 요청 결과

	ID_FG_PKT_REGIST_PLAYER_REQ = 2000,		//프론트 서버에서 필드 서버로 유저 등록
	ID_GF_PKT_REGIST_PLAYER_RES,			//프론트 서버에서 필드 서버로 유저 등록 결과

	ID_FG_PKT_CLOSE_PLAYER_REQ,				//프론트 서버에서 필드 서버로 유저 삭제
	ID_GF_PKT_CLOSE_PLAYER_RES,				//프론트 서버에서 필드 서버로 유저 삭제 결과

	ID_CF_PKT_CHOOSE_CHAR_REQ,				//캐릭터 선택
	ID_FC_PKT_CHOOSE_CHAR_RES,				//캐릭터 선택 요청 결과

	ID_CF_PKT_CREATE_CHAR_REQ,				//캐릭터 생성 요청
	ID_FC_PKT_CREATE_CHAR_RES,				//캐릭터 생성 요청 결과

	ID_CF_PKT_ENTER_CHANNEL_REQ,			//Channel enter
	ID_CF_PKT_ENTER_CHANNEL_RES,			//Channel enter

	ID_CF_PKT_CREATE_ROOM_REQ,				//방 생성 요청
	ID_FC_PKT_CREATE_ROOM_RES,				//방 생성 요청 결과

	ID_CF_PKT_LEAVE_ROOM_REQ,				//방 퇴장 요청
	ID_FC_PKT_LEAVE_ROOM_RES,				//방 퇴장 요청 결과

	ID_CF_PKT_ENTER_ROOM_REQ,				//방 입장 요청
	ID_FC_PKT_ENTER_ROOM_RES,				//방 입장 요청 결과

	ID_FC_PKT_NEW_USER_IN_ROOM,				//방 입장 유저 알림

	ID_CF_PKT_CHAT_ROOM_REQ,				//방 채팅 요청z
	ID_FC_PKT_CHAT_ROOM_RES,				//방 채팅 요청 결과

	ID_CF_PKT_READY_ROOM_REQ,				//플레이어 레디 상태 요청
	ID_FC_PKT_READY_ROOM_RES,				//플레이어 레디 결과

	ID_CF_PKT_START_ROOM_REQ,				//방 게임 시작 상태 요청
	ID_FC_PKT_START_ROOM_RES,				//방 게임 시작  결과

	ID_FC_PKT_RESULT_ROOM,					//방 게임  결과
	
	ID_CF_PKT_ROOM_LIST_REQ,				//방 리스트 요청
	ID_FC_PKT_ROOM_LIST_RES,				//방 리스트 요청

	ID_CF_PKT_USER_LIST_REQ,				//유저 리스트 요청
	ID_FC_PKT_USER_LIST_RES,				//유저 리스트 요청

	ID_CF_PKT_ALL_COMPLETE_NTY,				//준비 완료
	ID_FC_PKT_ALL_COMPLETE,					//모두 준비완료

	ID_CF_PKT_MOVE_REQ,						//이동 요청
	ID_FC_PKT_MOVE_RES,						//이동 요청 결과

	ID_CF_PKT_LOBBY_CHAT_NTY,				//로비 채팅 

	ID_CF_PKT_AUTO_START_REQ,

	ID_FC_HELLO_WORLD,						//로비 입장 성공


};



struct PKT_REGIST_REQ : HEADER
{
	BYTE Type;
};


/*
typedef enum _RELAY_PROTOCOL
{
	ID_RG_PKT_REGIST_REQ = 9000,
	ID_GR_PKT_REGIST_RES,

	ID_CR_LOGIN_REQ,
	ID_RC_LOGIN_RES,

	ID_CR_ENTER_ROOM_REQ,
	ID_RC_ENTER_ROOM_RES,

	ID_CR_BROADCAST_NTY,

};

//////////////////////////////////////////////////////////////////////

//릴레이서버에서 GS서버로 접속요청
struct RF_PKT_REGIST_REQ : PKT_REGIST_REQ
{
	DWORD Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &Id;
	}

	RF_PKT_REGIST_REQ()
	{
		MainId = ID_RG_PKT_REGIST_REQ;
	}
};
//GS서버에서 릴레이서버로 접속요청 결과
struct FR_PKT_REGIST_RES  : HEADER
{
	WORD Result;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
	}

	FR_PKT_REGIST_RES()
	{
		MainId = ID_GR_PKT_REGIST_RES;
	}
};*/
//////////////////////////////////////////////////////////////////////

//step.1
//필드서버에서 프론트서버로 접속요청
struct GF_PKT_REGIST_REQ : PKT_REGIST_REQ
{
	DWORD Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &Id;
	}

	GF_PKT_REGIST_REQ()
	{
		MainId = ID_GF_PKT_REGIST_REQ;
	}
};
//프론트서버에서 필드서버로 접속요청 결과
struct FG_PKT_REGIST_RES  : HEADER
{
	WORD Result;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
	}

	FG_PKT_REGIST_RES()
	{
		MainId = ID_FG_PKT_REGIST_RES;
	}
};


//step.2
//클라이언트에서 프론트 서버로 인증 요청
struct CF_PKT_REGIST_REQ : PKT_REGIST_REQ
{
	wstring SessionKey;
	wstring Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &SessionKey;
		ar &Id;
	}

	CF_PKT_REGIST_REQ()
	{
		MainId = ID_CF_PKT_REGIST_REQ;
	}
};
//프론트서버에서 클라이언트로 인증 요청 결과 전송 성공하면 캐릭터 리스트 전송
struct FC_PKT_REGIST_RES  : HEADER
{
	WORD Result;
	wstring CharName;	

	enum ErrorCode
	{
		SUCCESS = 0,
		FAILED	= 1
	};

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &CharName;
	}

	FC_PKT_REGIST_RES()
	{
		MainId = ID_FC_PKT_REGIST_RES;
	}
};


//클라이언트에서 프론트 서버로 캐릭터 생성 요청
struct CF_PKT_CREATE_CHAR_REQ  : HEADER
{
	wstring CharName;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &CharName;
	}
	CF_PKT_CREATE_CHAR_REQ()
	{
		MainId = ID_CF_PKT_CREATE_CHAR_REQ;
	}
};

//프론트서버에서 클라이언트로 캐릭터 생성 요청 결과
struct FC_PKT_CREATE_CHAR_RES  : HEADER
{
	wstring CharName;
	WORD Result;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &CharName;
		ar &Result;
	}

	FC_PKT_CREATE_CHAR_RES()
	{
		MainId = ID_FC_PKT_CREATE_CHAR_RES;
	}
};


//step.3
//클라이언트에서 프론트 서버로 캐릭터 선택 요청
struct CF_PKT_CHOOSE_CHAR_REQ  : HEADER
{
	wstring CharName;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &CharName;
	}
	CF_PKT_CHOOSE_CHAR_REQ()
	{
		MainId = ID_CF_PKT_CHOOSE_CHAR_REQ;
	}
};

//클라이언트에서 프론트 서버로 캐릭터 선택 요청 결과
struct FC_PKT_CHOOSE_CHAR_RES  : HEADER
{
	wstring CharName;
	WORD Result;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &CharName;
		ar &Result;
	}

	FC_PKT_CHOOSE_CHAR_RES()
	{
		MainId = ID_FC_PKT_CHOOSE_CHAR_RES;
	}
};

//프론트서버에서 필드 서버로 캐릭터 선택 요청
struct FG_PKT_REGIST_PLAYER_REQ  : HEADER
{
	WORD ChannelId;
	WORD Index;
	wstring Account;
	wstring CharName;
	
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &ChannelId;
		ar &Index;
		ar &Account;
		ar &CharName;
	}

	FG_PKT_REGIST_PLAYER_REQ()
	{
		MainId = ID_FG_PKT_REGIST_PLAYER_REQ;

	}
};

//필드서버에서 프론트 서버로 캐릭터 선택 요청 결과
struct GF_PKT_REGIST_PLAYER_RES  : HEADER
{
	WORD Result;			//1 exist character duplication

	DWORD m_Index;

	wstring CharName;
	wstring Account;

	WORD ChannelId;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &m_Index;
		ar &CharName;
		ar &Account;
		ar &ChannelId;
	}

	GF_PKT_REGIST_PLAYER_RES()
	{
		MainId = ID_GF_PKT_REGIST_PLAYER_RES;
	}
};

//step 4 log out

struct CF_PKT_CLOSE_REQ  : HEADER
{
	wstring Account;
	wstring CharName;
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Account;
		ar &CharName;
	}

	CF_PKT_CLOSE_REQ()
	{
		MainId = ID_CF_PKT_CLOSE_REQ;
	}
};


struct CF_PKT_CLOSE_RES  : HEADER
{
	wstring Account;
	wstring CharName;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Account;
		ar &CharName;
	}

	CF_PKT_CLOSE_RES()
	{
		MainId = ID_CF_PKT_CLOSE_RES;
	}
};



struct FG_PKT_CLOSE_PLAYER_REQ  : HEADER
{
	WORD ChannelId;
	wstring Account;
	wstring CharName;
	

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &ChannelId;
		ar &Account;
		ar &CharName;
	}

	FG_PKT_CLOSE_PLAYER_REQ()
	{
		MainId = ID_FG_PKT_CLOSE_PLAYER_REQ;
	}
};

struct GF_PKT_CLOSE_PLAYER_RES  : HEADER
{
	WORD Result;
	WORD ChannelId;
	wstring Account;
	wstring CharName;


	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &ChannelId;
		ar &Account;
		ar &CharName;
	
	}

	GF_PKT_CLOSE_PLAYER_RES()
	{
		MainId = ID_GF_PKT_CLOSE_PLAYER_RES;
	}
};

////////////////////////////////////////////////////////////////////////////
struct CF_PKT_ENTER_CHANNEL_REQ  : HEADER
{
	WORD ChannelId;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &ChannelId;
	}

	CF_PKT_ENTER_CHANNEL_REQ()
	{
		MainId =  ID_CF_PKT_ENTER_CHANNEL_REQ;
	}
};

struct FC_PKT_ENTER_CHANNEL_RES  : HEADER
{
	WORD ChannelId;
	wstring Name;
	WORD Result;	
			
	enum ErrorCode
	{
		SUCCESS = 0,
		FAILED  = 1
	};

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &ChannelId;
		ar &Name;
		ar &Result;
	}

	FC_PKT_ENTER_CHANNEL_RES()
	{
		MainId = ID_CF_PKT_ENTER_CHANNEL_RES;
	}
};


////////////////////////////////////////////////////////////////////////////
struct CF_PKT_CREATE_ROOM_REQ  : HEADER
{
	BYTE  Type;
	WORD ChannelId;
	BYTE  MaxCount;
	wstring Name;
	wstring Passwd;


	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &ChannelId;
		ar &MaxCount;
		ar &Name;
		ar &Passwd;
	}
	
	CF_PKT_CREATE_ROOM_REQ()
	{
		MainId = ID_CF_PKT_CREATE_ROOM_REQ;
	}
};

struct FC_PKT_CREATE_ROOM_RES  : HEADER
{
	wstring Name;
	WORD Result;					//success 0 failed 1 max room count 2 not exist Game Type;
	DWORD RoomNumber;
	BYTE  MaxCount;

	enum ErrorCode
	{
		SUCCESS = 0,
		LIMIT_COUNT,
		NOT_EXIST_GAME_TYPE,
		NOT_FOUND_PLAYER
	};

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Name;
		ar &Result;
		ar &RoomNumber;
	    ar &MaxCount;
	}

	FC_PKT_CREATE_ROOM_RES()
	{
		MainId = ID_FC_PKT_CREATE_ROOM_RES;
	}
};

struct CF_PKT_LEAVE_ROOM_REQ  : HEADER
{
	WORD RoomNumber;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &RoomNumber;
	}

	CF_PKT_LEAVE_ROOM_REQ()
	{
		MainId = ID_CF_PKT_LEAVE_ROOM_REQ;
	}
};

struct FC_PKT_LEAVE_ROOM_RES  : HEADER
{
	enum ErrorCode
	{
		SUCCESS,
		FAILED
	};
	WORD Result;
	wstring PlayerName;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &PlayerName;
	}

	FC_PKT_LEAVE_ROOM_RES()
	{
		MainId = ID_FC_PKT_LEAVE_ROOM_RES;
	}
};

struct CF_PKT_ENTER_ROOM_REQ  : HEADER
{
	WORD RoomNumber;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &RoomNumber;
	}

	CF_PKT_ENTER_ROOM_REQ()
	{
		MainId = ID_CF_PKT_ENTER_ROOM_REQ;
	}
};

struct FC_PKT_ENTER_ROOM_RES  : HEADER
{
	WORD Result;							//0 success
	wstring PlayerName[MAX_TRNASMISSION_COUNT];
	int Pos[MAX_TRNASMISSION_COUNT];
	RoomInfo roominfo;

	enum ErrorCode
	{
		SUCCESS,
		FAILED
	};

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		for(int i=0;i<MAX_TRNASMISSION_COUNT;i++)
		{
			ar &PlayerName[i];
			ar &Pos[i];
		}
		ar &roominfo;
	}

	FC_PKT_ENTER_ROOM_RES()
	{
		MainId = ID_FC_PKT_ENTER_ROOM_RES;
	}
};

struct FC_PKT_NEW_USER_IN_ROOM  : HEADER
{
	int Pos;
	wstring PlayerName;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Pos;
		ar &PlayerName;
	}

	FC_PKT_NEW_USER_IN_ROOM()
	{
		MainId = ID_FC_PKT_NEW_USER_IN_ROOM;
	}
};

struct CF_PKT_CHAT_ROOM_REQ  : HEADER
{
	wstring Chat;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Chat;
	}

	CF_PKT_CHAT_ROOM_REQ()
	{
		MainId = ID_CF_PKT_CHAT_ROOM_REQ;
	}
};

struct FC_PKT_CHAT_ROOM_RES  : HEADER
{
	wstring Chat;
	WORD  Result;
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Chat;
		ar &Result;
	}

	FC_PKT_CHAT_ROOM_RES()
	{
		MainId = ID_FC_PKT_CHAT_ROOM_RES;
	}
};

struct CF_PKT_READY_ROOM_REQ  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CF_PKT_READY_ROOM_REQ()
	{
		MainId = ID_CF_PKT_READY_ROOM_REQ;
	}
};

struct FC_PKT_READY_ROOM_RES  : HEADER
{
	WORD Result;
	std::wstring Name;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &Name;
	}

	FC_PKT_READY_ROOM_RES()
	{
		MainId = ID_FC_PKT_READY_ROOM_RES;
	}
};

struct CF_PKT_START_ROOM_REQ  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CF_PKT_START_ROOM_REQ()
	{
		MainId = ID_CF_PKT_START_ROOM_REQ;
	}
};

struct FC_PKT_START_ROOM_RES  : HEADER
{
	WORD Result;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
	}

	FC_PKT_START_ROOM_RES()
	{
		MainId = ID_FC_PKT_START_ROOM_RES;
	}
};

struct FC_PKT_RESULT_ROOM  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	FC_PKT_RESULT_ROOM()
	{
		MainId = ID_FC_PKT_RESULT_ROOM;
	}
};


struct CF_PKT_ROOM_LIST_REQ  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CF_PKT_ROOM_LIST_REQ()
	{
		MainId = ID_CF_PKT_ROOM_LIST_REQ;
	}
};

struct FC_PKT_ROOM_LIST_RES  : HEADER
{
	RoomInfo info[MAX_TRNASMISSION_COUNT];

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		for(int i=0;i<MAX_TRNASMISSION_COUNT;i++)
			ar & info[i];
	}

	FC_PKT_ROOM_LIST_RES()
	{
		MainId = ID_FC_PKT_ROOM_LIST_RES;
	}
};

struct CF_PKT_USER_LIST_REQ  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CF_PKT_USER_LIST_REQ()
	{
		MainId = ID_CF_PKT_USER_LIST_REQ;
	}
};

struct FC_PKT_USER_LIST_RES  : HEADER
{
	UserInfo info;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		//for(int i=0;i<MAX_TRNASMISSION_COUNT;i++)
			ar & info;
	}

	FC_PKT_USER_LIST_RES()
	{
		MainId = ID_FC_PKT_USER_LIST_RES;
	}
};
	
struct CF_PKT_ALLCOMPLETE_NTY  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CF_PKT_ALLCOMPLETE_NTY()
	{
		MainId = ID_CF_PKT_ALL_COMPLETE_NTY;
	}
};

struct FC_PKT_ALLCOMPLETE  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	FC_PKT_ALLCOMPLETE()
	{
		MainId = ID_FC_PKT_ALL_COMPLETE;
	}
};
	
struct CF_PKT_MOVE_REQ  : HEADER
{
	double x,y,z;
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &x;
		ar &y;
		ar &z;
	}

	CF_PKT_MOVE_REQ()
	{
		MainId = ID_CF_PKT_MOVE_REQ;
	}
};

struct FC_PKT_MOVE_RES  : HEADER
{
	double x,y,z;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &x;
		ar &y;
		ar &z;	
	}

	FC_PKT_MOVE_RES()
	{
		MainId = ID_FC_PKT_MOVE_RES;
	}
};	


struct CF_PKT_AUTO_START_REQ  : HEADER
{
	WORD GameType;
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &GameType;
	}

	CF_PKT_AUTO_START_REQ()
	{
		MainId = ID_CF_PKT_AUTO_START_REQ;
	}
};
	
struct CF_PKT_LOBBY_CHAT_NTY  : HEADER
{
	wstring Msg;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Msg;
	}

	CF_PKT_LOBBY_CHAT_NTY()
	{
		MainId = ID_CF_PKT_LOBBY_CHAT_NTY;
	}
};	

struct FC_HELLO_WORLD  : HEADER
{
	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		
	}

	FC_HELLO_WORLD()
	{
		MainId = ID_FC_HELLO_WORLD;
	}
};


	
	

	
	

	
	

	
	

			
