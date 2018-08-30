#include "StdAfx.h"
#include "GSIniContext.h"

namespace GSFrontServer	{	namespace GSIniContext	{

GSIniContext::GSIniContext(void)
{
	m_Port = 0;
	m_MaxUser = 0;
	m_Id = -1;
}


GSIniContext::~GSIniContext(void)
{
}

VOID GSIniContext::Load(WCHAR *IniName)
{
	m_Port    = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 10000,IniName);
	m_MaxUser = GetPrivateProfileInt(_T("SERVER"), _T("MAXUSER"), 100,IniName);
	m_Id	  = GetPrivateProfileInt(_T("SERVER"), _T("ID"), 100,IniName);	
	GetPrivateProfileString(_T("SERVER"), _T("NAME"),_T(""),m_Name,sizeof(m_Name),IniName);

	GetPrivateProfileString(_T("SERVER"), _T("GAME_DB"),_T(""),m_DSNGameDB,sizeof(m_DSNGameDB),IniName);
	GetPrivateProfileString(_T("SERVER"), _T("ACCOUNT_DB"),_T(""),m_DSNAccount,sizeof(m_DSNAccount),IniName);
	GetPrivateProfileString(_T("SERVER"), _T("SQL_ID"),_T(""),m_Account,sizeof(m_Account),IniName);
	GetPrivateProfileString(_T("SERVER"), _T("SQL_PASSWD"),_T(""),m_Passwd,sizeof(m_Passwd),IniName);


	m_UnityPort    = GetPrivateProfileInt(_T("SERVER"), _T("UNITY_SERVER_PORT"), 9000,IniName);
	GetPrivateProfileString(_T("SERVER"), _T("UNITY_SERVER_IP"),_T(""),m_UnityServerIp,sizeof(m_UnityServerIp),IniName);
}

}	}