#pragma once

#include "stdafx.h"
#include "common.h"
#include "proto_header.h"
#include "Front.h"

typedef enum _RELAY_PROTOCOL
{
	ID_RG_PKT_REGIST_REQ = 9000,
	ID_GR_PKT_REGIST_RES,

	ID_CR_LOGIN_REQ,
	ID_RC_LOGIN_RES,

	ID_CR_ENTER_ROOM_REQ,
	ID_RC_ENTER_ROOM_RES,

	ID_CR_LEAVE_ROOM_REQ,
	ID_RC_LEAVE_ROOM_RES,

	ID_CR_MY_INFO_NTY,

	ID_CR_BROADCAST_NTY,

	ID_CR_CREATE_UDP_REQ,
	ID_RC_CREATE_UDP_RES,

	ID_RC_REQUEST_STUN,

	ID_CR_CHANGE_MODE,
	ID_RC_CHANGE_MODE

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
};

//릴레이서버에서 GS서버로 접속요청
struct CR_PKT_LOGIN_REQ : PKT_REGIST_REQ
{
	DWORD Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &Id;
	}

	CR_PKT_LOGIN_REQ()
	{
		MainId = ID_CR_LOGIN_REQ;
	}
};
//GS서버에서 릴레이서버로 접속요청 결과
struct RC_PKT_LOGIN_RES  : HEADER
{
	WORD Result;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
	}

	RC_PKT_LOGIN_RES()
	{
		MainId = ID_RC_LOGIN_RES;
	}
};

//릴레이서버에서 GS서버로 접속요청
struct CR_PKT_ENTER_ROOM_REQ : PKT_REGIST_REQ
{
	DWORD Id;
	DWORD PlayerIndex;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &Id;
		ar &PlayerIndex;
	}

	CR_PKT_ENTER_ROOM_REQ()
	{
		MainId = ID_CR_ENTER_ROOM_REQ;
	}
};

struct RC_PKT_ENTER_ROOM_RES : PKT_REGIST_REQ
{
	DWORD Id;
	WORD Result;
	WORD Port;
	WORD GroupID;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Type;
		ar &Id;
		ar &Result;
		ar &Port;
		ar &GroupID;
	}

	RC_PKT_ENTER_ROOM_RES()
	{
		MainId = ID_RC_ENTER_ROOM_RES;
	}
};

//GS서버에서 릴레이서버로 접속요청 결과
struct CR_PKT_LEAVE_ROOM_REQ  : HEADER
{
	DWORD Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Id;
	}

	CR_PKT_LEAVE_ROOM_REQ()
	{
		MainId = ID_CR_LEAVE_ROOM_REQ;
	}
};
struct RC_PKT_LEAVE_ROOM_RES  : HEADER
{
	WORD Result;
	WORD Port;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &Port;
	}

	RC_PKT_LEAVE_ROOM_RES()
	{
		MainId = ID_RC_LEAVE_ROOM_RES;
	}
};

struct CR_PKT_BROADCAST_NTY  : HEADER
{
	DWORD Id;
	char MSG[1024];

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Id;
		ar &MSG;
	}

	CR_PKT_BROADCAST_NTY()
	{
		MainId = ID_CR_BROADCAST_NTY;
	}
};

//GS서버에서 릴레이서버로 접속요청 결과
struct CR_PKT_CREATE_UDP_REQ  : HEADER
{
	DWORD Id;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Id;
	}

	CR_PKT_CREATE_UDP_REQ()
	{
		MainId = ID_CR_CREATE_UDP_REQ;
	}
};

struct RC_PKT_UDP_CREATE_UDP_RES  : HEADER
{
	WORD Id;
	WORD GroupId;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Id;
		ar &GroupId;
	}

	RC_PKT_UDP_CREATE_UDP_RES()
	{
		MainId = ID_RC_CREATE_UDP_RES;
	}
};

struct RC_PKT_UDP_REQUEST_STUN  : HEADER
{
	WORD Result;

	WCHAR SrcIP[20];
	WORD  SrcPort;

	WCHAR DestIP[20];
	WORD  DestPort;

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &SrcIP;
		ar &SrcPort;
		ar &DestIP;
		ar &DestPort;
	}

	RC_PKT_UDP_REQUEST_STUN()
	{
		MainId = ID_RC_REQUEST_STUN;
	}
};

struct CR_CHANGE_MODE  : HEADER
{

	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	CR_CHANGE_MODE()
	{
		MainId = ID_CR_CHANGE_MODE;
	}
};

struct RC_CHANGE_MODE  : HEADER
{
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
	}

	RC_CHANGE_MODE()
	{
		MainId = ID_RC_CHANGE_MODE;
	}
};












