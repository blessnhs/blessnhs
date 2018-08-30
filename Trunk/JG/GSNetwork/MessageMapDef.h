#define DECLARE_MSG_MAP(SESSIONNAME)	\
typedef void (SESSIONNAME::*NETFUNC)(LPVOID,DWORD);	\
	std::map<DWORD,NETFUNC> FuncList;

#define ADD_NET_FUNC(SESSIONNAME,ID,FUNCTION)	\
	FuncList[ID] = &SESSIONNAME::FUNCTION;	\

#define NET_FUNC_EXE(ID,MSG,SIZE)	\
	std::map<DWORD,NETFUNC>::iterator iter = FuncList.find(ID);	\
	if(iter != FuncList.end())	\
		(this->*iter->second)((LPVOID)MSG,SIZE);