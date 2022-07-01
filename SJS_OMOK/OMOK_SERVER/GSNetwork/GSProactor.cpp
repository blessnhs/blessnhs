#include "StdAfx.h"
#include "GSProactor.h"


namespace GSFrames	{	namespace GSProactor	{

GSProactor::GSProactor(GSProactorImpl *ProactorImpl)
{
	m_ProactorImpl = ProactorImpl;
}

GSProactor::~GSProactor(void)
{
	delete m_ProactorImpl;
}

bool GSProactor::Create(int InputTheadCnt)
{
	return m_ProactorImpl->Create(InputTheadCnt);
}

bool GSProactor::Activate(int Type,int TCnt)
{
	return m_ProactorImpl->Activate(Type,TCnt);
}

bool GSProactor::RegisterCommand(IMessagePtr msg)
{
	return m_ProactorImpl->RegisterCommand(msg);
}

bool GSProactor::Register(BYTE Type,IMessagePtr Msg)
{
	return m_ProactorImpl->Register(Type,Msg);
}

bool GSProactor::Remove()
{
	return m_ProactorImpl->Remove();
}

bool GSProactor::Handle_Event(BYTE ProcId)
{
	return m_ProactorImpl->Handle_Event(ProcId);
}

}	}