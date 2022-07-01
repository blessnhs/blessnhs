#pragma once

#include "GSProactorImpl.h"


namespace GSFrames	{	namespace GSProactor	{

using namespace std;
typedef GSFrames::GSProactorImpl::GSProactorImpl GSProactorImpl;

class GSProactor : public GSGuard<GSProactor> 
{
public:
	GSProactor(GSProactorImpl *ProactorImpl);
	~GSProactor(void);

	virtual bool Create(int InputTheadCnt);
	virtual bool Activate(int Type,int TCnt);
	virtual bool RegisterCommand(IMessagePtr msg);
	
protected:

	virtual bool Register(BYTE Type,IMessagePtr Msg);
	virtual bool Remove();
	virtual bool Handle_Event(BYTE ProcId);

	GSProactorImpl	*m_ProactorImpl;

	
};

}	}

