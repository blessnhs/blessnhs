#pragma once

#include "stdafx.h"
#include "common.h"
#include "proto_header.h"

typedef enum _AUTH_PROTOCOL
{
	ID_AUTH_LOGIN_REQ = 30000,
	ID_AUTH_LOGIN_RES,
};

struct PKT_AUTHENCATION_REQ : HEADER
{
	PKT_AUTHENCATION_REQ()	
	{
		MainId = ID_AUTH_LOGIN_REQ;
	}
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Id;
		ar &Passwd;
		ar &Result;
	}

	wstring Id;
	wstring Passwd;

	WORD Result;
};

struct PKT_AUTHENCATION_RES : HEADER
{
	PKT_AUTHENCATION_RES()
	{
		MainId = ID_AUTH_LOGIN_RES;
	}
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& ar,const unsigned int version)
	{
		ar &Result;
		ar &SessionKey;
		ar &IpAddress;
		ar &Port;
		ar &Id;
	}

	WORD Result;
	wstring SessionKey;
	wstring IpAddress;
	WORD  Port;
	wstring Id;
};

	

			
