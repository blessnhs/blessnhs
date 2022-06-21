#pragma once

namespace GSNetwork	{	namespace GSObject	{	

class GSObject
{
public:
	GSObject(void);
	virtual ~GSObject(void);

	void SetId(unsigned long _Id)
	{
		m_Id = _Id; 
	}
	unsigned long GetId() { return m_Id; }

protected:

	unsigned long m_Id;
};

}	}

#define GSOBJECT GSNetwork::GSObject::GSObject 