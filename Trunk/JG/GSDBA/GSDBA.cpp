// GSDBA.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServerIOCP.h"
#include "DBProcess.h"
#include "GSResult.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);	
  
	if (!GSNetwork::GSMiniDump::GSMiniDump::Begin())
		return 0;

	GSResult *pResult = new GSResult;

	pResult->m_Result.push_back(_T("1"));
	pResult->m_Result.push_back(_T("hello"));

	s64 a;
	std::wstring str;

	*pResult >> a; 
	*pResult >> str; 
	
	GSDBA::CDBProcess::GetDBProcess().Initalize(_T("GS_ACCOUNT"),_T("GS_GAME"),_T("SQL_NHS"),_T("SQL_NHS"));

	GSDBA::CDBProcess::GetDBProcess().ProcedureUserLogin();
	
	GSDBA::CServerIOCP::CServerIOCP ServerIocp;
	if (ServerIocp.Begin())
	{
		getchar();
		ServerIocp.End();
	}

	WSACleanup();
	GSNetwork::GSMiniDump::GSMiniDump::End();
	CoUninitialize();
}

