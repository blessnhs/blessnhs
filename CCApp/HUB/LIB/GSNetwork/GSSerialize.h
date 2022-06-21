#pragma once

#define DESERIALIZE_BEGIN(DATA,STRUCT,STRUCTNAME,SIZE_)	\
	try\
{\
	std::stringstream ss;\
	ss.write((const char *)DATA,SIZE_);\
	boost::archive::text_iarchive ia(ss);\
	unsigned vERsion = ia.get_library_version();\
	STRUCT STRUCTNAME; \
	ia >> STRUCTNAME;	\

#define DESERIALIZE_END	\
} catch (...) { SYSLOG().Write("DESERIALIZE ERROR\n");return ; }\

class GSSerialize
{
public:
	GSSerialize(void);
	~GSSerialize(void);

};

/////////////////////////////READ//////////////////////////////////////////////////////
#define DECLARE_JSON_READ	\
	Json::Value		root;	\
	Json::Reader	reader;	\
	bool bParse = reader.parse((char *)Data, root);	\
	if(!bParse) {	\
		SYSLOG().Write("Json String Parse Error...%s\r\n",Data);	\
		return;	\
	}	\

#define GET_JSON_WSTR_MEMBER(MEMBER,DATA) \
	if( root.isMember(MEMBER) == false) \
		return ;\
	std::string _##DATA = root.get(MEMBER,"").asString();	\
	std::wstring DATA;	\
	DATA.assign(_##DATA.begin(),_##DATA.end());	\

#define GET_JSON_INT_MEMBER(MEMBER,DATA) \
	if( root.isMember(MEMBER) == false) \
		return ;\
	if(root.get(MEMBER,-1).isInt() == false)\
		return ;\
	int DATA = root.get(MEMBER,-1).asInt();	\


#define GET_JSON_UINT_MEMBER(MEMBER,DATA) \
	if( root.isMember(MEMBER) == false) \
		return ;\
	if(root.get(MEMBER,-1).isString() == true)\
		return ;\
	unsigned int DATA = root.get(MEMBER,0).asUInt();	\

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////WRITE///////////////////////////////////////////////////
#define DECLARE_JSON_WRITER	\
	Json::Value root2;	\

#define ADD_JSON_MEMBER(MEMBER,DATA) \
	root2[MEMBER] = DATA;	\

#define ADD_JSON_WSTR_MEMBER(MEMBER,DATA) \
{\
	std::string VAR;	\
	VAR.assign(DATA.begin(),DATA.end());\
	root2[MEMBER] = VAR;	\
}\

#define END_JSON_MEMBER(OBJ,MAINID)	\
	Json::FastWriter writer;	\
	std::string outputConfig = writer.write( root2 );	\
	OBJ->WritePacket(MAINID,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); \
	////////////////////////////////////////////////////////////////////////////////////////

#define MTOW(VAR)	\
	std::wstring _VAR;	\
	_VAR.assign(VAR.begin(),VAR.end());	\


#define WTOM(VAR)	\
	std::string _##VAR;	\
	_##VAR.assign(VAR.begin(),VAR.end());	\

/*
std::string dest;\
dest.resize(SIZE_);\
std::copy((char*)DATA,(char*)DATA+SIZE_,begin(dest));\
std::stringstream ss(dest);\
boost::archive::text_iarchive ia(ss);\
unsigned vERsion = ia.get_library_version();\
STRUCT STRUCTNAME; \
ia >> STRUCTNAME;	\

*/