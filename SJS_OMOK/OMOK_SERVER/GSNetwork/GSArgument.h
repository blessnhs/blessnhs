#pragma once

class GSArgument
{
public:
	GSArgument(void);
	~GSArgument(void);

	//최대 접속 가능 클라이언트
	WORD m_MaxClient;	
	
	//tcp 바인드 포트
	WORD m_BindPort;

	//로직 쓰레드 카운트
	WORD m_LogicThreadCnt;

	//버전
	WORD m_Version;

	//나글 알고리즘
	BOOL m_Naggle;

	//얼라이브 타임 설정 second 
	DWORD m_AliveTime;

	//udp바인트 포트
	std::vector<DWORD> m_UdpPorts;

	//최대 db thread Count
	WORD m_DBThreadCnt;
};

