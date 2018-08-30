#include "StdAfx.h"
#include "GSIniContext.h"

namespace GSRelayServer	{	namespace GSIniContext	{

GSIniContext::GSIniContext(void)
{
	DWORD m_Port = 0;
	DWORD m_Id   = 0;
	memset(m_Name,0,sizeof(m_Name));

	m_UdpPort  = 0;
	m_UdpCount = 0;

	memset(GSIP,0,sizeof(GSIP));
	GSPort = 0;
	m_MaxPlayerCount = 0;
}


GSIniContext::~GSIniContext(void)
{
}

VOID GSIniContext::Load(WCHAR *IniName)
{
	m_Port    = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 10000,IniName);
	m_Id	  = GetPrivateProfileInt(_T("SERVER"), _T("ID"), 100,IniName);	
	GetPrivateProfileString(_T("SERVER"), _T("NAME"),_T(""),m_Name,sizeof(m_Name),IniName);
	m_MaxPlayerCount = GetPrivateProfileInt(_T("SERVER"), _T("MAX_PLAYER"), 100,IniName);	

	m_UdpCount  = GetPrivateProfileInt(_T("SERVER"), _T("UDP_COUNT"), 10000,IniName);
	m_UdpPort = GetPrivateProfileInt(_T("SERVER"), _T("UDP PORT"), 100,IniName);	

	GetPrivateProfileString(_T("GSSERVER"), _T("IP"),_T(""),GSIP,sizeof(GSIP),IniName);
	GSPort    = GetPrivateProfileInt(_T("GSSERVER"), _T("PORT"), 10000,IniName);

}

}	}