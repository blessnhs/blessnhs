#include "StdAfx.h"
#include "UDPSession.h"
#include "../GSPacket/Relay.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSSerialize.h"

bool UDP_Client::Connect(char *ip,WORD Port,WORD UID,WORD GROUPID)
{
//	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
//		printf("WSAStartUp() error!");

	MyId = UID;

	hSocket = socket(PF_INET,SOCK_DGRAM, 0);
	if(hSocket == INVALID_SOCKET)
		printf("socket() Error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	servAddr.sin_port = htons(Port);

	{

		char szName[256];
		int nSize = sizeof(szName);
		gethostname(szName, nSize);  // 일단 자기 컴퓨터의 이름을 얻고
		SOCKADDR_IN MyAddr;
		long int *add;
		struct in_addr *myen;

		hostent* myent = gethostbyname(szName); // 이름을 토대로 정보들을 얻는다.
		printf("%s\n", myent->h_name);

		while(*myent->h_addr_list != NULL)
		{
			add = (long int *)*myent->h_addr_list;
			MyAddr.sin_addr.s_addr = *add;
			printf("%s\n", inet_ntoa(MyAddr.sin_addr));
			myent->h_addr_list++;
		}

	}

	char buffer[1024];

	WORD MID = ID_RC_CREATE_UDP_RES;

	memcpy(buffer,&MID,sizeof(MID));

	DECLARE_JSON_WRITER
	ADD_JSON_MEMBER("Id",(WORD)UID)
	ADD_JSON_MEMBER("GroupIdx",GROUPID)

	Json::FastWriter writer;	
	std::string outputConfig = writer.write( root2 );	
	//OBJ->WritePacket(MAINID,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 

	memcpy(buffer+sizeof(MID),outputConfig.c_str(),outputConfig.size());

	sendto(hSocket,(char*)buffer,outputConfig.size() + sizeof(WORD),0,
		(struct sockaddr*)&servAddr, sizeof(servAddr));

	m_state = C2S_UDP_TRY;

	return true;
}

UDP_Client::~UDP_Client() 
{
	closesocket(hSocket);

	WSACleanup();
}

void UDP_Client::Proc()
{
	ProcRecv();
	ProcSend();
}

void UDP_Client::ProcRecv()
{
	if(m_state == NONE)
		return ;

	if(m_state == P2P_TUNNELING)
	{
		memset(message,0,BUFFSIZE);

		addr_size = sizeof(fromAddr);
		strLen = recvfrom(hSocket, message, BUFFSIZE, 0,
			(struct sockaddr*)&fromAddr, &addr_size);

		if(strLen > 1)
		{
			message[strLen] = 0;
			printf("서버가 보낸 메세지 : %s \n", message);

			m_state = P2P_COMPLETE;

		}

		Sleep(100);


	}
}

void UDP_Client::ProcSend()
{
	if(m_state == NONE)
		return ;

	if(m_state == P2P_TUNNELING)
	{
		{
			char *ch =  inet_ntoa(ToAddr.sin_addr); 
			sprintf(message,"%s_%d",ch,ToAddr.sin_port);

			printf("TUNNELING %s\n",message);

			for(int i=0;i<1;i++)
			{
				sendto(hSocket,message,strlen(message),0,
					(struct sockaddr*)&ToAddr, sizeof(ToAddr));
			}
	
			Sleep(100);

		}		
	}

	if(m_state == P2P_COMPLETE)
	{
		{
			char *ch =  inet_ntoa(ToAddr.sin_addr); 
			sprintf(message,"%s_%d",ch,ToAddr.sin_port);

			printf("TUNNELING %s\n",message);

			for(int i=0;i<1;i++)
			{
				sendto(hSocket,message,strlen(message),0,
					(struct sockaddr*)&ToAddr, sizeof(ToAddr));
			}

			Sleep(500);

		}	
	}
}
