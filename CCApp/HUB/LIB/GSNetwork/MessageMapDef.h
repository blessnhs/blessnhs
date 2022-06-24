#define DECLARE_MSG_MAP(SESSIONNAME)	\
typedef void (SESSIONNAME::*NETFUNC)(boost::shared_ptr<XDATA>,boost::shared_ptr<GSClient>);	\
	std::map<DWORD,NETFUNC> FuncList;

#define DECLARE_MSG_MAP2(SESSIONNAME)	\
typedef void (SESSIONNAME::*NETFUNC)(boost::shared_ptr<XDATA>,boost::shared_ptr<GSClient>);	\
	std::map<DWORD,NETFUNC> FuncList;

#define ADD_NET_FUNC(SESSIONNAME,ID,FUNCTION)	\
	FuncList[ID] = &SESSIONNAME::FUNCTION;	\

#define NET_FUNC_EXE(ID,MSG,SIZE,CLIENT)	\
	std::map<DWORD,NETFUNC>::iterator iter = FuncList.find(ID);	\
	if(iter != FuncList.end())	\
		(this->*iter->second)((LPVOID)MSG,SIZE,CLIENT);

#define NET_FUNC_EXE2(XBUFF,CLIENT)	\
	std::map<DWORD,NETFUNC>::iterator iter = FuncList.find(XBUFF->MainId);	\
	if(iter != FuncList.end())	\
		(this->*iter->second)(XBUFF,CLIENT);


#define DECLARE_RECV_TYPE(TYPE,NAME)	\
	TYPE NAME;	\
	bool _result_ = NAME.ParseFromArray(pBuffer->m_Buffer.GetBuffer(), pBuffer->Length);	\
	if (_result_ == false)	\
		return;	\



#define SEND_PROTO_BUFFER(NAME,SESSION,RESERVE2)	\
{	\
	if(SESSION->GetConnected() == TRUE)	\
	{	\
	size_t _bufSize__ = NAME.ByteSizeLong();	\
	BYTE *_arr_send_ = new BYTE[_bufSize__];	\
	google::protobuf::io::ArrayOutputStream os(_arr_send_, _bufSize__);	\
	NAME.SerializeToZeroCopyStream(&os);	\
	SESSION->GetTCPSocket()->WritePacket(NAME.id(), 0, _arr_send_, (DWORD)_bufSize__,RESERVE2);	\
	delete[] _arr_send_ ; \
	}	\
}\

#define SEND_PROTO_BUFFER2(SUB_ID,NAME,SESSION,RESERVE2)	\
{	\
	if(SESSION->GetConnected() == TRUE)	\
	{	\
	size_t _bufSize__ = NAME.ByteSizeLong();	\
	BYTE *_arr_send_ = new BYTE[_bufSize__];	\
	google::protobuf::io::ArrayOutputStream os(_arr_send_, _bufSize__);	\
	NAME.SerializeToZeroCopyStream(&os);	\
	SESSION->GetTCPSocket()->WritePacket(NAME.id(), SUB_ID, _arr_send_, (DWORD)_bufSize__,RESERVE2);	\
	delete[] _arr_send_ ; \
	}	\
}