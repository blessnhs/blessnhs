#pragma once
#include "StdAfx.h"

class CCS_PKT_AUTHENCATION2;
SERIALIZE_DEFINE_TAG(CCS_PKT_AUTHENCATION2, eTagUserClass);
SERIALIZE_DECLARE_PUTGET(CCS_PKT_AUTHENCATION2);

class CCS_PKT_AUTHENCATION2
{

public:
	CCS_PKT_AUTHENCATION2(void){}
	~CCS_PKT_AUTHENCATION2(void){}

	string Id;

	string Passwd;

	int Index;

	list<int> ilist;

	SERIALIZE_DECLARE_FRIEND(CCS_PKT_AUTHENCATION2);
}



SERIALIZE_DEFINE_PUT(CCS_PKT_AUTHENCATION2,obj,serializer);
{
	serializer.Put(obj.Id);
	serializer.Put(obj.Passwd);
	serializer.Put(obj.Index);
	serializer.Put(obj.ilist);
	return TRUE;
}


SERIALIZE_DEFINE_GET(CCS_PKT_AUTHENCATION2,obj,serializer);
{
	serializer.Get(obj.Id);
	serializer.Get(obj.Passwd);
	serializer.Get(obj.Index);
	serializer.Get(obj.ilist);
	return TRUE;
}
