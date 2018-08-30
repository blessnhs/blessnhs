#include "StdAfx.h"
#include "GSIniContext.h"

GSIniContext::GSIniContext(void)
{
	m_Port = 0;
	m_MaxUser = 0;
	m_Id = -1;

	memset(m_FontIp,0,sizeof(m_FontIp));
	m_FrontPort=0;
}


GSIniContext::~GSIniContext(void)
{
}

VOID GSIniContext::Load(WCHAR *IniName)
{
	
	m_Port    = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 0,IniName);
	m_MaxUser = GetPrivateProfileInt(_T("SERVER"), _T("MAXUSER"), 100,IniName);
	m_Id		  = GetPrivateProfileInt(_T("SERVER"),_T("ID"), 100,IniName);	

	m_FrontPort    = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 0,IniName);
	
	GetPrivateProfileString(_T("SERVER"), _T("NAME"), _T(""), m_Name,SERVER_NAME, IniName);

	GetPrivateProfileString(_T("FRONT_SERVER"), _T("IP"), _T("127.0.0.1"), m_FontIp,20, IniName);
}
