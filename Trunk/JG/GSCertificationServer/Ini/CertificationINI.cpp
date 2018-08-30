#include "StdAfx.h"

#include "CertificationINI.h"

VOID CertificationINI::Load()
{
	Port = GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 1982,_T("./Certification.ini"));
	MaxUser = GetPrivateProfileInt(_T("SERVER"), _T("MAXUSER"), 100,_T("./Certification.ini"));

	Default_FrontPort = GetPrivateProfileInt(_T("SERVER"), _T("DEFAULT_FRONT_PORT"), 100,_T("./Certification.ini"));

	GetPrivateProfileString(_T("SERVER"), _T("DEFAULT_FRONT_IP"),_T("127.0.0.1"),Default_FrontIp,sizeof(Default_FrontIp),_T("./Certification.ini"));

	printf("[ Certification ] Port [ %d ] MaxUser [ %d ] Front Ip = %S:%d \n",Port,MaxUser,Default_FrontIp,Default_FrontPort);
}