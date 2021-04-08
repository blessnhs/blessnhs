#include "StdAfx.h"
#include "MSG_PLAYER_QUERY_RES.h"

#include "ICommand.h"
#include "GSPacketTCP.h"
#include "IProcess.h"

//#include "../GSPacket/Protocol.h"

namespace GSNetwork	{
/*

	template<class T>
	class MSG_PLAYER_QUERY_RES : public IMessage
	{
	public:
		MSG_PLAYER_QUERY_RES()  {}
		~MSG_PLAYER_QUERY_RES() {}

		shared_ptr<T>		pResponse;
		SESSION		*pSession;
		WORD		Result;

		void Execute(LPVOID Param)
		{
			if(pSession != NULL)
			{
				pSession->OnResponse(pResponse);

				pSession->DecDBStamp();
			}


		}
		void Undo(){}
	};*/

}