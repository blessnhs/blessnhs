#pragma once

using namespace std;

class CCS_PKT_AUTHENCATION
{
public:
	CCS_PKT_AUTHENCATION(void){}
	~CCS_PKT_AUTHENCATION(void){}

	WCHAR Id;
	WCHAR Passwd;

	WORD Result;
};




/*
class CCS_PKT_AUTHENCATION
{
public:
	CCS_PKT_AUTHENCATION(void){}
	~CCS_PKT_AUTHENCATION(void){}

	wstring Id;
	wstring Passwd;

	WORD Result;

	wstring GetId()
	{
		return Id;
	}

	void SetId(wstring _Id)
	{
		Id = _Id;
	}

	wstring GetPasswd()
	{
		return Passwd;
	}

	void SetPasswd(wstring _Passwd)
	{
		Passwd = _Passwd;
	}

	void SetData(char *Data,DWORD size)
	{
		Serializer s;
		Buffer buf;
		buf.Read(Data,size);

		s.BeginReading(&buf, false );
		s.Get(*this);
		s.EndReading();
	}

	char* GetData(DWORD &Size)
	{
		Serializer s;
		Buffer buf;	
		s.BeginWriting(&buf, false);
	    s.Put(*this) ;
	    s.EndWriting() ;

		Size = buf.GetLength();
		return (char*)buf.GetData();
	}

  SERIALIZE_DECLARE_FRIEND(CCS_PKT_AUTHENCATION);
};

SERIALIZE_DEFINE_PUT(CCS_PKT_AUTHENCATION, obj, serializer)
{
	  return serializer.Put(obj.Id) &&
		serializer.Put(obj.Passwd) ;
}

SERIALIZE_DEFINE_GET(CCS_PKT_AUTHENCATION, obj, serializer)
{
   return serializer.Get(obj.Id) &&
    serializer.Get(obj.Passwd) ;
}
*/