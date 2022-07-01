#pragma once

#include "GSReactorImpl.h"


namespace GSFrames	{	namespace GSReactor	{

class GSReactor : public GSGuard<GSReactor> 
{
public:
	GSReactor(GSFrames::GSReactorImpl::GSReactorImpl *pimpl);
	~GSReactor(void);

	virtual int Initialize();
	virtual void Terminate();

	virtual bool Register(IMessagePtr Msg);
	virtual bool Remove(IMessagePtr Msg);

protected:

	GSFrames::GSReactorImpl::GSReactorImpl		*m_ReactorImpl;

};

}	}