#include "ICommand.h"
#include "./IMessage.h"
#include "GSProactorImpl.h"

namespace GSFrames	{	namespace GSProactorImpl	{

class GSExecuteArgument
{
public:
	GSExecuteArgument()		{	m_ProcactorImpl = NULL;	m_ExecuteType = 0;	}
	~GSExecuteArgument()	{}

	GSProactorImpl	*m_ProcactorImpl;
	int				m_ExecuteType;
	int				m_Id;

};

}	}