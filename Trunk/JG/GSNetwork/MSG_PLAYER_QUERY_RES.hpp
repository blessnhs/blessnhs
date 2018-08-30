
#include "ICommand.h"
#include "GSPacketTCP.h"
#include "IProcess.h"
#include "GSClient.h"


namespace GSNetwork	{

	template<class T>
	class MSG_PLAYER_QUERY_RES : public IMessage
	{
	public:
		MSG_PLAYER_QUERY_RES()  {}
		~MSG_PLAYER_QUERY_RES() {}

		boost::shared_ptr<T>		pResponse;
		GSCLIENT					*pSession;
		WORD						Result;

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