#pragma  once

#define BUFFSIZE 1024

class UDP_Client
{
public:

	enum State
	{
		NONE,
		C2S_UDP_TRY,
		P2P_TUNNELING,
		P2P_COMPLETE
	};

	UDP_Client() { m_state = NONE; }
	~UDP_Client() ;

	bool Connect(char *ip,WORD Port,WORD UID,WORD GROUPID);

	void Proc();
	void ProcRecv();
	void ProcSend();

	int m_state;

public:

	WSADATA wsaData;
	SOCKET hSocket;
	int strLen;
	int addr_size;

	SOCKADDR_IN servAddr;	
	SOCKADDR_IN fromAddr;
	SOCKADDR_IN ToAddr;

	DWORD MyPort;
	DWORD MyId;

	char message[BUFFSIZE];
};


