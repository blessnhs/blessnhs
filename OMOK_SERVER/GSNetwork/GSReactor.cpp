#include "StdAfx.h"
#include "GSReactor.h"



namespace GSFrames	{	namespace GSReactor	{


GSReactor::GSReactor(GSFrames::GSReactorImpl::GSReactorImpl *pimpl)
{
	m_ReactorImpl = pimpl;
}

GSReactor::~GSReactor(void)
{

}

int GSReactor::Initialize()
{
	return m_ReactorImpl->Initialize();
}

void GSReactor::Terminate()
{
	return m_ReactorImpl->Terminate();
}

bool GSReactor::Register(IMessagePtr Msg)
{
	return m_ReactorImpl->Register(Msg);
}

bool GSReactor::Remove(IMessagePtr Msg)
{
	return m_ReactorImpl->Remove(Msg);
}

}	}