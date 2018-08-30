
#include "ICommand.h"
#include "GSPacketTCP.h"
#include "IProcess.h"

#include "../../PLAYER/Container/PlayerContainer.h"
#include "../../DBProxy/DBProcess.h"
#include "../../DBProxy/DBJob/RequestPlayerAuth.h"

namespace GSFrontServer	{

	template<class T>
	class MSG_PLAYER_QUERY_RES : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY_RES()  {}
		~MSG_PLAYER_QUERY_RES() {}

		boost::shared_ptr<T>		pResponse;
		GSCLIENT	*pSession;
		WORD		Result;

		void Execute(LPVOID Param)
		{
			if(pSession != NULL)
			{
				PLAYERMGR.OnResponse(pResponse.get());

				pSession->DecDBStamp();
			}
		}
		void Undo(){}
	};

	template<>
	class MSG_PLAYER_QUERY_RES<VOID> : public IMESSAGE
	{
	public:
		MSG_PLAYER_QUERY_RES()  {}
		~MSG_PLAYER_QUERY_RES() {}

		boost::shared_ptr<RequestPlayerAuth>		pResponse;
		GSCLIENT	*pSession;
		WORD		Result;

		void Execute(LPVOID Param)
		{
			if(pSession != NULL)
			{
				pSession->OnResponse(pResponse.get());

				pSession->DecDBStamp();
			}
		}
		void Undo(){}
	};

}