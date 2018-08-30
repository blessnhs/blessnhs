
#include "stdafx.h"
#include "MSG_CLOSE.h"
#include "../GROUP/GroupContainer.h"
#include "../IOCP/UDP/ServerIOCP_UDP.h"

void MSG_CLOSE::Execute(LPVOID Param)
{
	if(pSession != NULL)
	{
		IProcess<TCP_SESSION> *pProcess = pSession->GetProcess();
		if(pProcess != NULL)
			delete pProcess;

		pSession->SetProcess(NULL);
	
		printf("socket disconnected\n");

		pSession->Recycle(pListen->GetSocket());

		pSession->SetConnected(FALSE);

		GROUP_PTR	pGroup = GROUPCONTAINER.Search(pSession->m_AssociateSocket.m_GroupNumber);
		if(pGroup != NULL)
		{
			pGroup->EraseByTCPId(pSession->GetId());

			if(pGroup->m_MemberList.size() == 0)
			{
				printf("Erase Group %d\n",pSession->m_AssociateSocket.m_GroupNumber);
				GROUPCONTAINER.Del(pGroup);

				for each( auto member in pGroup->m_ConnectionPairList)
				{
					if(member != NULL)
					{
						delete member;
					}
				}

				 pGroup->m_ConnectionPairList.clear();
			}

			printf("leave Group %d\n",pSession->m_AssociateSocket.m_GroupNumber);
		}

		if(pSession->m_AssociateSocket.m_AllocPort != 0)
		{
			IOCP_UDP.m_FreePorts.push_back(pSession->m_AssociateSocket.m_AllocPort);

			pSession->m_AssociateSocket.m_AllocPort = 0;
		}
	
	}

}
